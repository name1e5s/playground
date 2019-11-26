# 观察 IPCS 指令输出

IPCS 是 Linux 下用以提供IPC设备的信息的指令，其样例输出如下：

```bash
name1e5s@sumeru:~$ ipcs -a --human

------ Message Queues --------
key        msqid      owner      perms      size         messages

------ Shared Memory Segments --------
key        shmid      owner      perms      size       nattch     status
0x0052e2c1 196608     postgres   600           56B     6

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
```

由此可见，通过 IPCS 指令，我们可以查看当前使用的共享内存、消息队列及信号量等所有信息，对于该选项对应的结果，介绍以下几个部分：

1、信号量在创建时分信号量集和信号量的概念，该命令的查询结果中，`Semaphore Arrays` 下面每一行代表一个信号量集，其中 `perms` 对应信号量集的权限，`nsems` 对应信号量集中信号量的个数，对于信号量集的创建方法可以查询 `semctl` 相关的函数使用方法。

2、对于消息队列 `Message Queues` 而言，可以看到 `msqid` 对应创建队列时得到的 `id` 值，从 `messages` 中可以看到当前队列中存在的消息个数，从 `used_bytes` 中可以看到当前所有消息占用的字节数，所以单个消息的字节数则为总字节数除以消息数，同时如果消息个数不为零则说明消息队列中的消息没有得到及时处理，可以据此判断是否存在队列阻塞的风险。

// REF: https://blog.csdn.net/dalongyes/article/details/50616162