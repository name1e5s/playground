# 操作系统实验一 —— Linux 操作系统的安装

### 实验内容

在配备Windows操作系统 Host 机上，安装虚拟机软件 VMware Workstation ， 对硬盘进行分区和格式化，安装Linux操作系统，以便在一台机器上模拟出多种操作系统运行环境。

### 实验环境

主机操作系统版本： Windows 10 1903

Linux 发行版：Ubuntu 19.10 (*Eoan Ermine*)

VMware Workstation Pro 版本：15

### 虚拟机安装

1. 打开北邮人 BT，搜索 VMware Workstation Pro，获取[下载链接](https://bt.byr.cn/details.php?id=284764&hit=1) 。

2. 打开程序进行安装

   ![VMware Workstation Pro 15 安装界面](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig1-1.png)

3. 一路下一步进行程序的安装

4. 在安装的最后，选择许可证，输入神秘代码

   ```
   CG392-4PX5J-H816Z-HYZNG-PQRG2
   ```

5. 双击桌面上的 `VMware Workstation Pro` 图标，看到如下所示界面，表示虚拟机安装成功。

   ![VMware Workstation Pro 15 主界面](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig1-2.png)

### Ubuntu 19.10 的安装

1. 首先登陆 [清华大学开源软件镜像站](https://mirrors.tuna.tsinghua.edu.cn/) 获取 Ubuntu 19.10 的镜像

2. 打开 VMware Workstation Pro 15，选择创建虚拟机，按照我们的需求确定对应的虚拟机配置，因为 VMware Workstation Pro 对于 Ubuntu 等常见的 Linux 发行版提供了“简易安装”的选项，我们只需等待虚拟机自动为我们安装。需要注意的是，在 Ubuntu 的安装过程中如果联网可能会导致 Ubuntu 尝试进行更新，该过程通常会很慢，因此建议在安装过程中关闭互联网的连接。

   ![最终配置的结果](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-1.png)

   ![Ubuntu 安装界面](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-2.png)

3. Ubuntu 会在安装后的第一次重启安装 open-vm-tools 以支持一些方便使用的功能，因此第一次开机会比较慢

   ![open-vm-tools 的安装提示](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-3.png)

4. 在一些初始化工作结束后，我们就可以看到 Ubuntu 的登陆界面，可以注意到这一版本的登陆界面的背景色相比之前的版本比较浅

   ![Ubuntu 19.10 登陆界面](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-4.png)

5. 选择之前创建的账户并输入密码，即可进入 Ubuntu 桌面

   ![Ubuntu 19.10 主界面](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-5.png)

6. 按下 `Ctrl + Alt + T` 打开 Terminal，输入 `sudo sed -i 's/us.archive.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list` 修改软件源为清华源，之后通过 `sudo apt update && sudo apt upgrade` 同步软件库并进行更新

7. 准备 Linux 内核的编译环境，在终端内输入 `sudo apt build-dep linux linux-image-$(uname -r)` 获取所需的软件包，按照 [Ubuntu Wiki](https://wiki.ubuntu.com/Kernel/BuildYourOwnKernel#Build_Environment)  所述，我们还需要执行 `sudo apt install libncurses-dev flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf` 以安装编译内核所需的依赖。

8. 使用 `apt source linux-image-$(uname -r)` 即可获取我们后续需要修改的 Linux 源码

9. 最后，安装广受好评的代码编辑器 `VS Code`：

   ```
   wget https://go.microsoft.com/fwlink/?LinkID=760868 -O /tmp/code.deb && sudo apt install /tmp/code.deb
   ```

10. 至此，Linux 安装完毕

    ![Ubuntu With VS Code](https://raw.githubusercontent.com/name1e5s/playground/master/OS-Lab/fig/lab-1/fig2-6.png)