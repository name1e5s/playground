# 6.2 编写 DAEMON 进程

实现原理：通过 `fork` 生成新的子进程而后自杀来将子进程转交给 `init` 进程托管

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char** argv) {
    pid_t pid = 0;
    pid_t sid = 0;
    FILE *fp= NULL;
    int i = 0;
    pid = fork();

    if (pid < 0) {
        printf("fork failed!\n");
        exit(1);
    }

    if (pid > 0) {
       exit(0); //terminate the parent process
    }

    umask(0); //unmasking the file mode

    sid = setsid(); //set new session
    if(sid < 0) {
        exit(1);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        system("ps >> f1.txt");
        sleep(100);
    }

    return 0;
}
```

输出如下：

```bash
name1e5s@DESKTOP-D8BIQ3U:~$ ./a.out
name1e5s@DESKTOP-D8BIQ3U:~$ ls
Backup-for-favorites  a.out  f1.txt  s  t.c  test.c
name1e5s@DESKTOP-D8BIQ3U:~$ cat f1.txt
  PID TTY          TIME CMD
   99 ?        00:00:00 a.out
  100 ?        00:00:00 sh
  101 ?        00:00:00 ps
name1e5s@DESKTOP-D8BIQ3U:~$ cat f1.txt
```

当然使用 BSD 的 `daemon()` 函数也是一种实现方法，不过其限制较多且不与 POSIX 规范兼容，在此不表。

