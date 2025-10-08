main:
    lis $4
    .word 0xffff0004
    lis $5
    .word 0xffff000c
    lis $6
    .word 4
    lis $7
    .word -1
    lis $2
    .word 0

read:
    lw $1, 0($4)
    beq $1, $7, break
    sub $30, $30, $6
    sw $1, 0($30)
    add $2, $2, $6
    beq $0, $0, read

break:
    beq $2, $0, exit

print:
    lw $1, 0($30)
    sw $1, 0($5)
    add $30, $30, $6
    sub $2, $2, $6
    bne $2, $0, print

exit:
    jr $31