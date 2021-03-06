# Lab 1

> 兄啊，是你是 CPU 还是俺是 CPU 啊

首先要阅读 MIPSsim 模拟器的使用方法（见附录），然后了解 MIPSsim 的指令系统和汇
编语言。

(1) 启动 MIPSsim(用鼠标双击 MIPSsim．exe)。

(2) 选择“配置”—>“流水方式”选项，使模拟器工作在非流水方式下。

(3) 参照 MIPSsim 使用说明，熟悉 MIPSsim 模拟器的操作和使用方法。

可以先载人一个样例程序(在本模拟器所在的文件夹下的“样例程序”文件夹中)，然后
分别以单步执行一条指令、执行多条指令、连续执行、设置断点等的方式运行程序，观
察程序执行情况，观察 CPU 中寄存器和存储器的内容的变化。

(4)选择“文件”—> “载入程序”选项，加载样例程序 alltest.asm，然后查看“代码”
窗口，
至看程序所在的位置(起始地址为 Ox00000100)。

(5)查看“寄存器"窗口 PC 寄存器的值：[PC]=**0x00000000**

(6)执行 load 和 store 指令，步骤如下：

①单步执行 1 条指令(F7)。

②下一条指令地址为 **0x00000004**，是一条**有**(有，无)符号载入**字节**(字
节．半字，字)指令。

③单步执行 1 条指令(F7)。

④查看 R1 的值，[R1]=**0xFFFFFFFFFFFFFF80**

⑤下一条指令地址为 **0x00000008**，是一条**无**(有，无)符号载入**字**(字
节．半字，字)指令。

⑥单步执行 1 条指令。

⑦查看 R1 的值，[R1]=**0x0000000000000080**。

⑧下一条指令地址为 **0x0000000C**，是一条**无**(有，无)符号载入**字节**(字
节．半字，字)指令。

⑨单步执行 1 条指令。

⑩查看 R1 的值，[R1]=**0x0000000000000080**

⑾单步执行 1 条指令。

⑿下一条指令地址为 **0x00000014**，是一条保存**字**(字节．半字，字)指令。

⒀单步执行 1 条指令(F7)。

⒁查看内存 BUFFER 处字的值，值为 **0x80**。（内存—>符号表）

(7)执行算术运算类指令。步骤如下：

①双击“寄存器”窗口中的 R1，将其值修改为 2。

②双击“寄存器”窗口中的 R2，将其值修改为 3。

③单步执行 1 条指令。

④下一条指令地址为 **0x00000020**，是一条加法指令。

⑤单步执行 1 条指令。

⑥查看 R3 的值，[R3]=**0x0000000000000005**

⑦下一条指令地址为 **0x00000024**，是一条乘法指令。

⑧单步执行 1 条指令。

⑨查看 L0、HI 的值，[LO]=**0x0000000000000006**，[HI]=**0x0000000000000000**

(8)执行逻辑运算类指令。步骤如下：

①双击“寄存器”窗口中的 R1，将其值修改为 0xFFFF0000。

②双击“寄存器”窗口中的 R2，将其值修改为 0xFF00FF00。

③单步执行 1 条指令。

④下一条指令地址为 **0x00000030**，是一条逻辑与运算指令，第二个操作数寻址
方式是**寄存器直接寻址**(寄存器直接寻址，立即数寻址)。

⑤单步执行 1 条指令。

⑥查看 R3 的值，[R3]=**0x00000000FF000000**

⑦下一条指令地址为 **0x00000034**，是一条逻辑与运算指令，第二个操作数寻址
方式是**立即数寻址**（寄存器直接寻址，立即数寻址）。

⑧单步执行 1 条指令。

⑨查看 R3 的值，[R3]=**0x0000000000000000**

(9)执行控制转移类指令。步骤如下：

①双击“寄存器”窗口中的 R1，将其值修改为 2。

②双击“寄存器”窗口中的 R2，将其值修改为 2。

③单步执行 1 条指令。

④下一条指令地址为 **0x00000040**，是一条 BEQ 指令，其测试条件是
**$r1 == $r2**，
目标地址为 **0x0000004C**

⑤单步执行 1 条指令。

⑥查看 PC 的值，[PC]=**0x0000004C**，表明分支**成功**（成功，失败）。

⑦下一条指令是一条 BGEZ 指令，其测试条件是 **$r1>=0**，目标地址为
**0x00000058**

⑧单步执行 1 条指令。

⑨查看 PC 的值，[PC]=**0x00000058**，表明分支**成功**（成功，失败）。

⑩下一条指令是一条 BGEZAL 指令，其测试条件是 **$r1>=0**，目标地址为
**0x00000064**

⑾单步执行 1 条指令。

⑿查看 PC 的值[PC]=**0x00000064**，表明分支**成功**（成功，失败）；查看 R31
的值，[R31]=**0x0000005C**

⒀单步执行 1 条指令。

⒁查看 R1 的值，[R1]=**0x0000000000000074**

⒂下一条指令地址为 **0x00000068**，是一条 JALR 指令，保存目标地址的寄存器
为 R**1**，保存返回地址的目标寄存器为 R**3**。

⒃单步执行 1 条指令。

⒄查看 PC 和 R3 的值，[PC]=**0x00000074**，[R3]=**0x000000000000006C**