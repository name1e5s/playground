.text
main:
ADDIU  $r4,$r0,a
ADDIU  $r5,$r0,b
ADDIU  $r6,$r0,n
BGEZAL  $r0, naive_prod
NOP 
TEQ $r0,$r0

naive_prod:
LW     $r6, 0($r6)
ADD    $r8, $r0, $r0
ADD    $r2, $r0, $r0
loop:
LW     $r9, 0($r4)
LW     $r10, 0($r5)
MUL    $r11, $r9, $r10
ADD    $r2, $r2, $r11
ADDIU  $r4, $r4, 4
ADDIU  $r5, $r5, 4
ADDIU  $r8, $r8, 1
BNE    $r8, $r6, loop
JR     $r31

.data
a: 
.word 1,2,3,4,5,6,7,8,9,10,11
b:
.word 1,2,3,4,5,6,7,8,9,10,11
n:
.word 11
