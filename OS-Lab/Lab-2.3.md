# 实验 2.3  Linux 内核模块

### 编写内核模块

我们要实现的内核模块主要包括标记为 `__init` 的初始化函数以及标记为 `__exit` 的退出函数，还有描述文件相关的 `file_operations` 结构体以及用以处理读取/写入的相关函数。

因为 `do_gettimeofday` 函数在 `5.0` 以上的内核上被删除了，我们需要使用如下代码来模拟该函数的行为：

```C
void do_gettimeofday(struct timeval *tv) {
	struct timespec now;

	getnstimeofday(&now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec/1000;
}
```



整体代码如下：

```C
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("name1e5s");
MODULE_DESCRIPTION("A harmless clock.");
MODULE_VERSION("0.1");

static const char *file_name = "clock";

// Removed in kernel 5.0, use getnstimeofday instead.
// D**N IT
void do_gettimeofday(struct timeval *tv) {
	struct timespec now;

	getnstimeofday(&now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec/1000;
}

static int fill_info(struct seq_file *m, void *v) {
    struct timeval xtime;
    do_gettimeofday(&xtime);
    seq_printf(m, "%ld %ld\n", xtime.tv_sec, xtime.tv_usec);
	return 0;
}

static int open(struct inode *inode, struct file *file) {
	return single_open(file, fill_info, NULL);
}

static const struct file_operations fops = {
	.llseek = seq_lseek,
	.open = open,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static int __init clock_init(void) {
    printk(KERN_INFO "Clock: Hello from the harmless clock!\n");
    proc_create(file_name, 0, NULL, &fops);
    return 0;
}


static void __exit clock_exit(void){
    printk(KERN_INFO "Clock: Goodbye from the harmless clock!\n");
    remove_proc_entry(file_name, NULL);
}

module_init(clock_init);
module_exit(clock_exit);
```



在现代的 Linux 上面， 比较通用的可以编译该文件的 `Makefile` 如下：

```makefile
obj-m+=clock.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
```



运行结果如下：

```bash
name1e5s@ubuntu:~/lab2/2.3$ make
make -C /lib/modules/5.3.0-18-generic/build/ M=/home/name1e5s/lab2/2.3 modules
make[1]: Entering directory '/usr/src/linux-headers-5.3.0-18-generic'
  CC [M]  /home/name1e5s/lab2/2.3/clock.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/name1e5s/lab2/2.3/clock.mod.o
  LD [M]  /home/name1e5s/lab2/2.3/clock.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.3.0-18-generic'
name1e5s@ubuntu:~/lab2/2.3$ modinfo ./clock.ko
filename:       /home/name1e5s/lab2/2.3/./clock.ko
version:        0.1
description:    A harmless clock.
author:         name1e5s
license:        GPL
srcversion:     BE5E6FC2EF7FFD62690068C
depends:
retpoline:      Y
name:           clock
vermagic:       5.3.0-18-generic SMP mod_unload
name1e5s@ubuntu:~/lab2/2.3$ sudo insmod clock.ko
name1e5s@ubuntu:~/lab2/2.3$ cat /proc/clock
1572161313 290301
name1e5s@ubuntu:~/lab2/2.3$ sudo rmmod clock.ko
name1e5s@ubuntu:~/lab2/2.3$ cat /var/log/kern.log
Oct 27 00:28:23 ubuntu kernel: [17512.252405] Clock: Hello from the harmless clock!
Oct 27 00:28:39 ubuntu kernel: [17528.682741] Clock: Goodbye from the harmless clock!
name1e5s@ubuntu:~/lab2/2.3$
```

### 测试

测试代码使用指导书上的代码：

```C
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>

int main(void) {
	struct timeval getSystemTime;
	char procClockTime[256];
	int infile,len;

	gettimeofday(&getSystemTime,NULL);

	infile = open("/proc/clock",O_RDONLY);
	len = read(infile,procClockTime,256);
	close(infile);

	procClockTime[len] = '\0';

    printf("SystemTime is %d %d\nProcClockTime is %s\n",
        getSystemTime.tv_sec,
        getSystemTime.tv_usec,
        procClockTime);

	sleep(1);
    return 0;
} 
```

执行结果如下：

```bash
name1e5s@ubuntu:~/lab2/2.3$ gcc 2.3.c
2.3.c: In function ‘main’:
2.3.c:13:8: warning: implicit declaration of function ‘read’; did you mean ‘fread’? [-Wimplicit-function-declaratio ]
   13 |  len = read(infile,procClockTime,256);
      |        ^~~~
      |        fread
2.3.c:14:2: warning: implicit declaration of function ‘close’; did you mean ‘pclose’? [-Wimplicit-function-declaration]
   14 |  close(infile);
      |  ^~~~~
      |  pclose
2.3.c:18:28: warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘__time_t’ {aka ‘long int’} [-Wformat=]
   18 |     printf("SystemTime is %d %d\nProcClockTime is %s\n",
      |                           ~^
      |                            |
      |                            int
      |                           %ld
   19 |         getSystemTime.tv_sec,
      |         ~~~~~~~~~~~~~~~~~~~~
      |                      |
      |                      __time_t {aka long int}
2.3.c:18:31: warning: format ‘%d’ expects argument of type ‘int’, but argument 3 has type ‘__suseconds_t’ {aka ‘long int’} [-Wformat=]
   18 |     printf("SystemTime is %d %d\nProcClockTime is %s\n",
      |                              ~^
      |                               |
      |                               int
      |                              %ld
   19 |         getSystemTime.tv_sec,
   20 |         getSystemTime.tv_usec,
      |         ~~~~~~~~~~~~~~~~~~~~~
      |                      |
      |                      __suseconds_t {aka long int}
2.3.c:23:2: warning: implicit declaration of function ‘sleep’ [-Wimplicit-function-declaration]
   23 |  sleep(1);
      |  ^~~~~
name1e5s@ubuntu:~/lab2/2.3$ ./a.out
SystemTime is 1572161601 335857
ProcClockTime is 1572161601 335870
```

忽略由于给的示例代码不规范造成的 Warning 外，可以看出通过我们实现的模块获取的时间和使用 `gettimeofday` 函数获取的时间相差无几。