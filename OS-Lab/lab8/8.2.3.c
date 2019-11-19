#define __USE_UNIX98 // rwlock is a really fucking old lock
#include <pthread.h>
#include "8.2.common.h"

struct apple {
    unsigned long long a;
    unsigned long long b;
    pthread_rwlock_t rw_lock;
};

struct orange {
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
};

static struct apple apple_test;
static struct orange orange_test = {
        {0}, {0}
};

void *apple_operation_a(void *_unused) {
    pthread_rwlock_wrlock(&(apple_test.rw_lock));
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
    }
    pthread_rwlock_unlock(&(apple_test.rw_lock));
}

void *apple_operation_b(void *_unused) {
    pthread_rwlock_wrlock(&(apple_test.rw_lock));
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.b += sum;
    }
    pthread_rwlock_unlock(&(apple_test.rw_lock));
}

int main(void) {

    pthread_t apple_thread_a, apple_thread_b;
    pthread_create(&apple_thread_a, NULL, apple_operation_a, NULL);
    pthread_create(&apple_thread_b, NULL, apple_operation_a, NULL);
    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    pthread_join(apple_thread_a, NULL);
    pthread_join(apple_thread_b, NULL);
    return 0;
}