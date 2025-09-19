add $3, $1, $2
sub $4, $2, $1
mult $1, $2
div $2, $3
mflo $5
mfhi $6
lis $7
slt $8, $1, $2
jr $31
beq $1, $2, 16
lw $9, 20($10)
sw $11, 24($12)
