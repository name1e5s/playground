#define _GNU_SOURCE
#include <sched.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include "8.2.common.h"

struct apple {
    unsigned long long a;
    unsigned long long b;
};

struct orange {
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
};

static struct apple apple_test;
static struct orange orange_test = {
        {0}, {0}
};

void set_cpu_affinity(int i) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(i, &set);
    if (sched_setaffinity(gettid(), sizeof(set), &set) == -1) {
        exit(1);
    }
}

void *apple_operation_a(void *_unused) {
    set_cpu_affinity(1);
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
    }
}

void *apple_operation_b(void *_unused) {
    set_cpu_affinity(2);
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.b += sum;
    }
}

int main(void) {
    set_cpu_affinity(0);
    pthread_t apple_thread_a, apple_thread_b;
    pthread_create(&apple_thread_a, NULL, apple_operation_a, NULL);
    pthread_create(&apple_thread_b, NULL, apple_operation_b, NULL);
    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    pthread_join(apple_thread_a, NULL);
    pthread_join(apple_thread_b, NULL);
    return 0;
}