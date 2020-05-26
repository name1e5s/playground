# Flexisip 安装

本次我们选用的 SIP 服务端为 Flexisip，其提供了较为简单的搭建 SIP 服务器的方式，而且其配套客户端 UI 比较好看。

### Prerequisites

本次实验的服务端我们选择在阿里云主机上进行搭建，操作系统为 `Ubuntu 18.04 LTS`。

### 安装

首先增加对应的软件源。

在 `/etc/apt/source.list`上增加如下代码：

```
# For Ubuntu 18.04 LTS
deb [arch=amd64] http://linphone.org/snapshots/ubuntu bionic stable beta # alpha
```

之后需要信任软件源，添加 PGP Key

```
wget https://www.linphone.org/snapshots/ubuntu/pubkey.gpg -O - | sudo apt-key add -
```

之后即可直接安装

```
sudo apt update

sudo apt install bc-flexisip
```

### 配置

#### 生成默认配置文件

使用如下指令生成默认配置文件，以 root 权限执行如下指令

```
/opt/belledonne-communications/bin/flexisip --dump-default all > /etc/flexisip/flexisip.conf
```

#### 编写配置文件

之后我们打开 `/etc/flexisip/flexisip.conf`，删除全部内容，把如下配置复制到配置文件中：

```roboconf
[global]
debug=true

aliases=sip.name1e5s.fun


[module::Registrar]
enabled=true
reg-domains=sip.name1e5s.fun
db-implementation=internal

[module::Authentication]
enabled=true
auth-domains=sip.name1e5s.fun
db-implementation=file
datasource=/etc/flexisip/users.db.txt
```

这里面 `sip.name1e5s.fun` 为个人域名。

之后创建 `/etc/flexisip/users.db.txt`，输入账户配置：

```
version:1

test@sip.name1e5s.fun clrtxt:test ;
test2@sip.name1e5s.fun clrtxt:test ;
```

其中 `test` 为用户 ID， `clrtxt:` 后，空格前的为密码。

### 运行

以 root 权限执行如下指令：

```
/opt/belledonne-communications/bin/flexisip
```

看到如下内容即表示服务器运行成功，可以开始通话：

```

```
