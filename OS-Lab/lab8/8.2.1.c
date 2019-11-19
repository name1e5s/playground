#include "8.2.common.h"

struct apple {
    unsigned long long a;
    unsigned long long b;
};

struct orange {
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
};

int main(void) {
    static struct apple apple_test;
    static struct orange orange_test = {
        {0}, {0}
    };

    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
        apple_test.b += sum;
    }

    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    return 0;
}