# 实验 2.2  Linux 内核定时器

### A 用定时器 **ITIMER_REAL** 实现 **gettimeofday** 的功能。使其一秒钟产生一个信号，计算已经过的秒数。

#### ITIMER 介绍

间隔定时器（Interval Timer，简称 ITIMER）是一种使用间隔值作为计时方式的定时器。在定时器启动后，其间隔值不断地减小，当间隔值减小到  0 的时候，我们就将之视为到期。在 Linux 内核内，为每个 task 都提供了三个间隔定时器：

1. **ITIMER_REAL**： 在启动该计时器后，无论进程是否在运行，内核都会在每个时钟滴答将其间隔计数器减1，当减到0值时，内核向进程发送 SIGALRM 信号。
2. **ITIMER_VIRTUAL**：只有在进程在用户态执行时该计时器内的计数器才会自减，当减到 0 时，内核向进程发送 SIGVTALRM 信号。
3. **ITIMER_PROF**：只有在进程在执行时（用户态与内核态均可）该计时器内的计数器才会自减，当减到 0 时，内核向进程发送 SIGPROF信号。

#### 代码

```c
#include <stdio.h>
#include <signal.h>

#include <sys/time.h>

static void sighandle(int);

static int second = 0;

static void sighandle(int sig) {
    second ++;
    printf("%d\n", second);
}

int main(void) {
    // Set SIGALRM handler
    signal(SIGALRM, sighandle);

    // Fill the fucking struct
    struct itimerval timerval;
	timerval.it_interval.tv_sec = 1;
	timerval.it_interval.tv_usec = 0;
	timerval.it_value.tv_sec = 1;
	timerval.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timerval, NULL);
    
    // Geadloop
    while(1);
}
```

程序的输出结果如下：

```bash
name1e5s@ubuntu:~/lab2$ gcc 2.2.1.c
name1e5s@ubuntu:~/lab2$ ./a.out
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
^C
name1e5s@ubuntu:~/lab2$
```

### B 记录一个进程运行时所占用的**real time, cpu time,user time ,kernel time**。

代码：

```C
#include <sys/time.h>
#include <stdio.h>
#include <signal.h>

static void sighandle(int);

static long realsecond = 0;
static long vtsecond = 0;
static long profsecond = 0;

static void sighandle(int sig) {
    switch(sig) {
        case SIGALRM:
            realsecond += 10;
            break;
        case SIGVTALRM:
            vtsecond += 10;
            break;
        case SIGPROF:
            profsecond += 10;
            break;
        default:
            break;
    }
}

int fib(int n) {
    if(n == 0 || n == 1) {
        return 1;
    }

    return fib(n - 1) + fib(n - 2);
}

void print_time(const struct itimerval *tv, long second, const char *prompt) {
    long sec_count = 10 - tv->it_value.tv_sec;
	long msec_count = (1000000 - tv->it_value.tv_usec)/1000;
	printf("%s = %ld sec, %ld msec\n", prompt, second + sec_count,msec_count);
}

int main(void) {
    static struct itimerval realt,virtt,proft;

    // Set signal handler
    signal(SIGALRM, sighandle);
    signal(SIGVTALRM, sighandle);
    signal(SIGPROF, sighandle);

    // Fill the fucking struct
    struct itimerval timerval;
	timerval.it_interval.tv_sec = 10;
	timerval.it_interval.tv_usec = 0;
	timerval.it_value.tv_sec = 10;
	timerval.it_value.tv_usec = 0;

    // Set the fucking timer
    setitimer(ITIMER_REAL, &timerval, NULL);
	setitimer(ITIMER_VIRTUAL,&timerval,NULL);
	setitimer(ITIMER_PROF,&timerval,NULL);

    // Waste time...
    fib(22);
    for (int i = 0; i < 20000; i++) {
        puts("I Can Eat Glass and It Won't Hurt Me");
    }

    // Get the timer
	getitimer(ITIMER_PROF,&proft);
	getitimer(ITIMER_REAL,&realt);
	getitimer(ITIMER_VIRTUAL,&virtt);

    // Print time
    print_time(&realt, realsecond, "Real Time");
    print_time(&proft, profsecond, "CPU Time");
    print_time(&virtt, vtsecond, "User Time");

    // Calculate Kernel Time and Print
    long t1 = (10 - proft.it_value.tv_sec)*1000 + (1000000 - proft.it_value.tv_usec)/1000 + profsecond*10000;
	long t2 = (10 - virtt.it_value.tv_sec)*1000 + (1000000 - virtt.it_value.tv_usec)/1000 + vtsecond*10000;
	long moresec = (t1 - t2)/1000;
	long moremsec = (t1 - t2) % 1000;
	printf("Kernel Time = %ld sec, %ld msec\n", moresec, moremsec);

    return 0;
}
```

输出结果如下：

```bash
name1e5s@ubuntu:~/lab2$ gcc 2.2.2.c
name1e5s@ubuntu:~/lab2$ ./a.out | tail
I Can Eat Glass and It Won't Hurt Me
I Can Eat Glass and It Won't Hurt Me
I Can Eat Glass and It Won't Hurt Me
I Can Eat Glass and It Won't Hurt Me
I Can Eat Glass and It Won't Hurt Me
I Can Eat Glass and It Won't Hurt Me
Real Time = 1 sec, 45 msec
CPU Time = 1 sec, 40 msec
User Time = 0 sec, 1000 msec
Kernel Time = 0 sec, 40 msec
name1e5s@ubuntu:~/lab2$
```

### C、编写一个主程序产生两个子进程，分别低轨计算N =20，30，36的 Fibonacci 序列。分别对三个进程计算相应的 real time, cpu time,user time ,kernel time。

代码：

```C
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sighandle(int);

long realsecond = 0;
long vtsecond = 0;
long profsecond = 0;

void sighandle(int sig) {
    switch(sig) {
        case SIGALRM:
            realsecond += 10;
            break;
        case SIGVTALRM:
            vtsecond += 10;
            break;
        case SIGPROF:
            profsecond += 10;
            break;
        default:
            break;
    }
}

int fib(int n) {
    if(n == 0 || n == 1) {
        return 1;
    }

    return fib(n - 1) + fib(n - 2);
}

void print_time(const struct itimerval *tv, long second, const char *prompt, int n) {
    long sec_count = 10 - tv->it_value.tv_sec;
	long msec_count = (1000000 - tv->it_value.tv_usec)/1000;
	printf(prompt, n, second + sec_count, msec_count);
}

int fib_forked(int n) {
    struct itimerval realt,virtt,proft;

    // Set signal handler
    signal(SIGALRM, sighandle);
    signal(SIGVTALRM, sighandle);
    signal(SIGPROF, sighandle);

    // Fill the fucking struct
    struct itimerval timerval;
	timerval.it_interval.tv_sec = 10;
	timerval.it_interval.tv_usec = 0;
	timerval.it_value.tv_sec = 10;
	timerval.it_value.tv_usec = 0;

    // Set the fucking timer
    setitimer(ITIMER_REAL, &timerval, NULL);
	setitimer(ITIMER_VIRTUAL,&timerval,NULL);
	setitimer(ITIMER_PROF,&timerval,NULL);

    // Waste time...
    // Print info
    printf("fib(%d) = %d \n", n, fib(n));

    // Get the timer
	getitimer(ITIMER_PROF,&proft);
	getitimer(ITIMER_REAL,&realt);
	getitimer(ITIMER_VIRTUAL,&virtt);

    // Print time
    print_time(&realt, realsecond, "Fib(%d) Real Time = %ld sec, %ld msec\n", n);
    print_time(&proft, profsecond, "Fib(%d) CPU Time = %ld sec, %ld msec\n", n);
    print_time(&virtt, vtsecond, "Fib(%d) User Time = %ld sec, %ld msec\n", n);

    // Calculate Kernel Time and Print
    long t1 = (10 - proft.it_value.tv_sec)*1000 + (1000000 - proft.it_value.tv_usec)/1000 + profsecond*10000;
	long t2 = (10 - virtt.it_value.tv_sec)*1000 + (1000000 - virtt.it_value.tv_usec)/1000 + vtsecond*10000;
	long moresec = (t1 - t2)/1000;
	long moremsec = (t1 - t2) % 1000;
	printf("Kernel Time = %ld sec, %ld msec\n", moresec, moremsec);

    return 0;
}

int main(void) {
    int pid_fib10 = fork();
    if(pid_fib10 == 0) {
        fib_forked(10);
    } else {
        int pid_fib20 = fork();
        if(pid_fib20 == 0) {
            fib_forked(20);
        } else {
            int pid_fib36 = fork();
            if(pid_fib36 == 0) {
                fib_forked(36);
            }
        }
    }
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}
```

输出结果如下：

```bash
name1e5s@ubuntu:~/lab2$ gcc 2.2.3.c
name1e5s@ubuntu:~/lab2$ ./a.out
fib(10) = 89
Fib(10) Real Time = 1 sec, 0 msec
Fib(10) CPU Time = 0 sec, 996 msec
Fib(10) User Time = 0 sec, 996 msec
Kernel Time = 0 sec, 0 msec
fib(20) = 10946
Fib(20) Real Time = 1 sec, 0 msec
Fib(20) CPU Time = 0 sec, 996 msec
Fib(20) User Time = 0 sec, 996 msec
Kernel Time = 0 sec, 0 msec
fib(36) = 24157817
Fib(36) Real Time = 1 sec, 109 msec
Fib(36) CPU Time = 1 sec, 104 msec
Fib(36) User Time = 1 sec, 104 msec
Kernel Time = 0 sec, 0 msec
name1e5s@ubuntu:~/lab2$
```

