print:
    sw $1, -4($30)
    sw $2, -8($30)
    sw $3, -12($30)
    sw $4, -16($30)
    sw $5, -20($30)
    sw $6, -24($30)
    sw $7, -28($30)
    sw $31, -32($30)
    lis $2
    .word 128
    sub $30, $30, $2
    lis $4
    .word 0xffff000c
    lis $5
    .word 10
    lis $6
    .word 48
    add $7, $30, $0
    add $3, $0, $0
    beq $1, $0, zero
    slt $2, $1, $0
    beq $2, $0, extract
    lis $2
    .word 45
    sw $2, 0($4)

extract:
    loop:
        div $1, $5
        mfhi $2
        mflo $1
        slt $5, $2, $0
        beq $5, $0, remainder
        sub $2, $0, $2
    remainder:
        add $2, $2, $6
        sw $2, 0($7)
        lis $2
        .word 4
        add $7, $7, $2
        lis $2
        .word 1
        add $3, $3, $2
        lis $5
        .word 10
        bne $1, $0, loop

digits:
    beq $3, $0, newline
    lis $2
    .word 4
    sub $7, $7, $2
    lw $2, 0($7)
    sw $2, 0($4)
    lis $2
    .word 1
    sub $3, $3, $2
    bne $3, $0, digits

newline:
    sw $5, 0($4)
    beq $0, $0, restore

zero:
    add $2, $6, $0
    sw $2, 0($4)
    sw $5, 0($4)
    beq $0, $0, restore

restore:
    lis $2
    .word 128
    add $30, $30, $2
    lw $31, -32($30)
    lw $7, -28($30)
    lw $6, -24($30)
    lw $5, -20($30)
    lw $4, -16($30)
    lw $3, -12($30)
    lw $2, -8($30)
    lw $1, -4($30)
    jr $31
