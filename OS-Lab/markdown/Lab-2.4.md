# 实验 2.4 Linux 系统调用

## 关于 Linux 更新导致的代码变更的说明

1. CVE-2009-2009 以后，Linux 开始使用 `SYSCALL_DEFINEx` 系列的宏以确保传参的安全
2. `do_gettimeofday` 已死，我们使用 `getnstimeofday` 模拟其行为
3. `sys_call_table[]` 现在使用可读性更强的 `.tbl` 文件实现，其位置为 `arch/x86/entry/syscall/syscall_64.tbl`
4. `_syscall1` 宏已被弃用，我们使用 `syscall` 函数来调用我们自己添加的系统调用

## 添加新调用

1. 在 kernel/sys.c 内添加如下代码：

   ```C
   void do_gettimeofday(struct timeval *tv) {
   	struct timespec now;
   	getnstimeofday(&now);
   	tv->tv_sec = now.tv_sec;
   	tv->tv_usec = now.tv_nsec/1000;
   }
   
   int do_mysyscall(struct timeval *tv) {
   	struct timeval kernel_tv;
   	do_gettimeofday(&kernel_tv);
   	return copy_to_user(tv, &kernel_tv, sizeof(kernel_tv));
   }
   
   SYSCALL_DEFINE1(mysyscall, struct timeval *, tv)
   {
   	return do_mysyscall(tv);
   }
   
   ```

2. 在 `arch/x86/entry/syscall/syscall_64.tbl` 里面添加如下代码：

   ```
   436	common	mysyscall			__x64_sys_mysyscall
   ```

3. 使用 Ubuntu 提供的 `make-kpkg` 工具编译内核：

   ```bash
   fakeroot make-kpkg --initrd --append-to-version=-lab kernel-image kernel-headers -j8
   ```

4. 安装生成的 `.deb` 文件并重启切换到新内核

5. 使用如下代码进行测试：

   ```C
   #define _GNU_SOURCE
   #include <unistd.h>
   #include <sys/syscall.h>
   #include <sys/types.h>
   int main(int argc, char *argv[]) {
       struct timeval gettime;
   	struct timeval mycalltime;
   
   	gettimeofday(&gettime,NULL);
   	syscall(436, &mycalltime);
   			printf("gettimeofday:%d%d\n",gettime.tv_sec,gettime.tv_usec);
   	printf("mycall:%d%d\n",mycalltime.tv_sec,mycalltime.tv_usec);
   }
   ```

   结果如下：

   ```C
   name1e5s@ubuntu:~/lab2$ gcc 2.4.c 
   name1e5s@ubuntu:~/lab2$ ./a.out 
   gettimeofday:1572172804385006
   mycall:1572172804385008
   ```