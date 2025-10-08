main:
    lis $30
    .word 0x01000000
    lis $4
    .word 0xffff000c
    lis $2
    .word 0xDEADBEEF

    lis $1
    .word 0
    lis $11
    .word print
    jalr $11

    lis $1
    .word 241
    lis $11
    .word print
    jalr $11

    lis $1
    .word -123456789
    lis $11
    .word print
    jalr $11

    lis $1
    .word -2147483648
    lis $11
    .word print
    jalr $11

    lis $1
    .word 2147483647
    lis $11
    .word print
    jalr $11

halt:
    beq $0, $0, halt
