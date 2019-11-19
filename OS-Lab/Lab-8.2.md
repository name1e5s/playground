# MP

### 8.2.1

```bash
name1e5s@asgard:~$ cat /proc/cpuinfo | grep 'physical id' | sort | uniq | wc -l
1
name1e5s@asgard:~$ cat /proc/cpuinfo | grep 'cpu cores' | uniq | awk -F ':' '{print $2}'
 4
name1e5s@asgard:~$ cat /proc/cpuinfo | grep 'processor' | wc -l
8
```

### 8.2.2

#### Common Header

```C
#ifndef _COMMON
#define _COMMON

#define ORANGE_MAX_VALUE 1000000
#define APPLE_MAX_VALUE 100000000
#define MSECOND 1000000

#endif
```

#### Naive Program
```C
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
```

Result:
```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc 8.2.1.c -o 8.2.1
name1e5s@asgard:/data/playground/OS-Lab$ ls
8.2.1    8.2.common.h  Lab-1.md    Lab-2.2.md  Lab-2.4.md  Lab-3.2.md  Lab-4.1.1.md  Lab-6.2.md  Lab-7.2.sh
8.2.1.c  fig           Lab-2.1.md  Lab-2.3.md  Lab-3.2.c   lab4        Lab-4.1.2.md  Lab-7.2.md  Lab-8.2.md
name1e5s@asgard:/data/playground/OS-Lab$ ./8.2.1 
name1e5s@asgard:/data/playground/OS-Lab$ time ./8.2.1 

real    0m0.196s
user    0m0.189s
sys     0m0.004s
```

#### Two Thread

```C
#include <pthread.h>
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

void *apple_operation(void *_unused) {
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
        apple_test.b += sum;
    }
}

int main(void) {

    pthread_t apple_thread;
    pthread_create(&apple_thread, NULL, apple_operation, NULL);
    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    pthread_join(apple_thread, NULL);
    return 0;
}
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc 8.2.2.c -o 8.2.2 -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ls
8.2.1.c  8.2.2.c       fig       Lab-2.1.md  Lab-2.3.md  Lab-3.2.c   lab4          Lab-4.1.2.md  Lab-7.2.md  Lab-8.2.md
8.2.2    8.2.common.h  Lab-1.md  Lab-2.2.md  Lab-2.4.md  Lab-3.2.md  Lab-4.1.1.md  Lab-6.2.md    Lab-7.2.sh
name1e5s@asgard:/data/playground/OS-Lab$ ./8.2.2 
name1e5s@asgard:/data/playground/OS-Lab$ time ./8.2.2 

real    0m0.214s
user    0m0.227s
sys     0m0.004s
```

#### Three Threads
##### With Lock

```C
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
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc 8.2.3.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ./a.out 
name1e5s@asgard:/data/playground/OS-Lab$ time ./a.out 

real    0m0.341s
user    0m0.344s
sys     0m0.004s
```

##### Lock Free
```C
#include <pthread.h>
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

void *apple_operation_a(void *_unused) {
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
    }
}

void *apple_operation_b(void *_unused) {
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.b += sum;
    }
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
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc 8.2.3.lock_free.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ./a.out 
name1e5s@asgard:/data/playground/OS-Lab$ time ./a.out 

real    0m0.613s
user    0m1.219s
sys     0m0.004s
```

#### Cache Optimization
##### Lock Free
```C
#include <pthread.h>
#include "8.2.common.h"

struct apple {
    unsigned long long a __attribute__((aligned(LEVEL1_DCACHE_LINESIZE)));
    unsigned long long b __attribute__((aligned(LEVEL1_DCACHE_LINESIZE)));
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
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
    }
}

void *apple_operation_b(void *_unused) {
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.b += sum;
    }
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
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE` 8.2.4.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ./a.out 
name1e5s@asgard:/data/playground/OS-Lab$ time ./a.out 

real    0m0.502s
user    0m0.957s
sys     0m0.008s
```
##### With Lock
```C
#define __USE_UNIX98 // rwlock is a really fucking old lock
#include <pthread.h>
#include "8.2.common.h"

struct apple {
    unsigned long long a __attribute__((aligned(LEVEL1_DCACHE_LINESIZE)));
    unsigned long long b __attribute__((aligned(LEVEL1_DCACHE_LINESIZE)));;
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
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE` 8.2.4.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ./a.out 
name1e5s@asgard:/data/playground/OS-Lab$ time ./a.out 

real    0m0.479s
user    0m0.908s
sys     0m0.000s
```

#### CPU AFF
##### Two Thread
```C
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

void *apple_operation(void *_unused) {
    set_cpu_affinity(1);
    for(unsigned long long sum = 0; sum < APPLE_MAX_VALUE; sum ++) {
        apple_test.a += sum;
        apple_test.b += sum;
    }
}

int main(void) {
    set_cpu_affinity(0);
    pthread_t apple_thread;
    pthread_create(&apple_thread, NULL, apple_operation, NULL);
    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    pthread_join(apple_thread, NULL);
    return 0;
}
```

```bash
name1e5s@asgard:/data/playground/OS-Lab$ gcc 8.2.5.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab$ ./a.out 
name1e5s@asgard:/data/playground/OS-Lab$ time a.out
a.out: command not found

real    0m0.094s
user    0m0.081s
sys     0m0.013s
name1e5s@asgard:/data/playground/OS-Lab$ time ./a.out

real    0m0.228s
user    0m0.228s
sys     0m0.004s
```

##### Three Thread
```C
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
    pthread_create(&apple_thread_b, NULL, apple_operation_a, NULL);
    unsigned long long result = 0ULL;

    for(unsigned long long index = 0; index < ORANGE_MAX_VALUE; index ++) {
        result += orange_test.a[index] + orange_test.b[index];
    }
    
    pthread_join(apple_thread_a, NULL);
    pthread_join(apple_thread_b, NULL);
    return 0;
}
```

```bash
name1e5s@asgard:/data/playground/OS-Lab/lab8$ gcc 8.2.5.2.c -lpthread
name1e5s@asgard:/data/playground/OS-Lab/lab8$ time ./a.out 

real    0m0.360s
user    0m0.353s
sys     0m0.004s
```