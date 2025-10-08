stirling:
    sw $1, -4($30)
    sw $2, -8($30)
    sw $4, -12($30)
    sw $5, -16($30)
    sw $6, -20($30)
    sw $7, -24($30)
    sw $31, -28($30)
    lis $6
    .word 64
    sub $30, $30, $6
    beq $1, $0, nzero
    beq $2, $0, kzero
    beq $0, $0, rec
nzero:
    beq $2, $0, bothzero
    add $3, $0, $0
    beq $0, $0, restore
bothzero:
    lis $3
    .word 1
    beq $0, $0, restore
kzero:
    add $3, $0, $0
    beq $0, $0, restore
rec:
    sw $2, 4($30)
    lis $5
    .word 1
    sub $4, $1, $5
    sw $4, 0($30)
    add $1, $0, $4
    lw $2, 4($30)
    lis $7
    .word stirling
    jalr $7
    sw $3, 8($30)
    lw $4, 0($30)
    lw $2, 4($30)
    lis $5
    .word 1
    sub $2, $2, $5
    add $1, $0, $4
    lis $7
    .word stirling
    jalr $7
    lw $4, 0($30)
    lw $5, 8($30)
    mult $4, $5
    mflo $6
    add $3, $6, $3
restore:
    lis $6
    .word 64
    add $30, $30, $6
    lw $31, -28($30)
    lw $7, -24($30)
    lw $6, -20($30)
    lw $5, -16($30)
    lw $4, -12($30)
    lw $2, -8($30)
    lw $1, -4($30)
    jr $31
