# execve() 系统调用主线

在 POSIX 规范中，存在如下几个 exec 系列的函数:

```C
int      execl(const char *, const char *, ...);
int      execle(const char *, const char *, ...);
int      execlp(const char *, const char *, ...);
int      execv(const char *, char * const *);
int      execve(const char *, char * const *, char * const *);
int      execvp(const char *, char * const *);
```

在 Linux 下， 这几个函数最终都是通过内核提供的 execve 调用完成其功能的。在 Linux 内核内，该系统调用的声明如下：

```C
SYSCALL_DEFINE3(execve,
		const char __user *, filename,
		const char __user *const __user *, argv,
		const char __user *const __user *, envp)
{
	return do_execve(getname(filename), argv, envp);
}
```

在 2014 年 Linux 内核还增加了一个 5 个参数的 `execveat`，主要用于处理相对路径的问题，在此不表。

execve() 调用会直接调用位于 `fs/exec.c` 的 `do_execve` 函数，再经过层层中转，到达实际执行功能的 `__do_execve_file` 函数。其大致执行流程如下：

1. 判断当前的进程号是否超限
2. 取消不必要的文件共享
3. 使用 `kzalloc` 动态分配 `linux_binprm` 结构体并置为 0
4. 使用 `do_open_execat` 函数打开文件，并确定该文件是否可执行
5. 对于多处理机，使用 `sched_exec` 函数判断最近执行该进程的 CPU 是否可以执行新的程序并进行进程迁移等操作
6. 对于 `execveat` 操作，处理文件路径
7. 使用 `prepare_binprm` 填充之前创建的 `linux_binprm` 结构体，其大致流程为：
   1. 判断文件是否为可执行文件（至少可执行位应被设置）
   2. 初始化 `uid`， `gid` 变量
   3. 用可执行文件的前 128 字节复制到结构体的 buffer 内，以此确认文件的格式等信息
8. 分配新页，复制进程的相关信息
9. 使用 `exec_binprm` 函数尝试执行文件，其最终会调用文件对应的 `load_binary` 函数，该函数实际上创建了新的进程映像并覆盖旧的进程映像

