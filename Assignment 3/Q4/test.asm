main:
    lis $30
    .word 0x01000000
    lis $4
    .word 0xffff000c
    lis $8
    .word 0

    lis $1
    .word 0
    lis $2
    .word 0
    lis $11
    .word stirling
    jalr $11
    lis $5
    .word 1
    bne $3, $5, markfail

    lis $1
    .word 6
    lis $2
    .word 1
    lis $11
    .word stirling
    jalr $11
    lis $5
    .word 120
    bne $3, $5, markfail

    lis $1
    .word 7
    lis $2
    .word 3
    lis $11
    .word stirling
    jalr $11
    lis $5
    .word 1624
    bne $3, $5, markfail

    lis $1
    .word 10
    lis $2
    .word 5
    lis $11
    .word stirling
    jalr $11
    lis $5
    .word 0x00041c0d
    bne $3, $5, markfail

    lis $1
    .word 10
    lis $2
    .word 7
    lis $11
    .word stirling
    jalr $11
    lis $5
    .word 0x000024ea
    bne $3, $5, markfail

    beq $8, $0, pass
    beq $0, $0, fail

markfail:
    lis $8
    .word 1
    beq $0, $0, fail

pass:
    lis $10
    .word 80
    sw $10, 0($4)
    lis $10
    .word 65
    sw $10, 0($4)
    lis $10
    .word 83
    sw $10, 0($4)
    sw $10, 0($4)
    lis $10
    .word 10
    sw $10, 0($4)
    beq $0, $0, halt

fail:
    lis $10
    .word 70
    sw $10, 0($4)
    lis $10
    .word 65
    sw $10, 0($4)
    lis $10
    .word 73
    sw $10, 0($4)
    lis $10
    .word 76
    sw $10, 0($4)
    lis $10
    .word 10
    sw $10, 0($4)

halt:
    beq $0, $0, halt
