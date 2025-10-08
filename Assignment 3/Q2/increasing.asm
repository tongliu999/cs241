main:
    lis $10
    .word 4
    lis $6
    .word 1
    lis $3
    .word 1
    add $28, $31, $0
    add $13, $1, $0
    sub $8, $2, $6
    add $8, $8, $8
    add $8, $8, $8
    add $14, $1, $8

loop:
    beq $13, $14, exit
    lw $1, 0($13)
    add $5, $13, $10
    lw $2, 0($5)
    lis $12
    .word compare
    jalr $12
    beq $3, $0, dec
    add $13, $13, $10
    beq $0, $0, loop

dec:
    lis $3
    .word 0
    add $31, $28, $0
    jr $31

exit:
    add $31, $28, $0
    jr $31
