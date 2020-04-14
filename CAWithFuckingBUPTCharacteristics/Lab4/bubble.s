.text
main:
ADDIU  $r4,$r0,a
ADDIU  $r5,$r0,n
LW     $r5, 0($r5)
BGEZAL  $r0, bubble
NOP 
TEQ $r0,$r0

bubble:
ADDIU  $r7, $r5, -1
BLEZ   $r7, exit
SLL    $r5, $r5, 2
ADDIU  $r8, $r4, 4
ADDU   $r6, $r4, $r5
loop:
ADDIU  $r2, $r8, 0
run:
LW     $r3, -4($r2)
LW     $r4, 0($r2)
SLT    $r5, $r4, $r3
BEQ    $r5, $r0, end
swap:
SW     $r4, -4($r2)
SW     $r3, 0($r2)
end:
ADDIU  $r2, $r2, 4
BNE    $r6, $r2, run

ADDIU  $r7, $r7, -1
ADDIU  $r6, $r6, -4
BNE    $r7, $r0, loop

exit:
JR     $r31

.data
a: 
.word 11,10,9,8,7,6,5,4,3,2,1
n:
.word 11
