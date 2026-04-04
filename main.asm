.data

.text
.globl main

main:
    addi $0, $2, 59
    addi $0, $1, 3
    add $3, $2, $1
    add $3, $2, $1
    addi $0, $4, 10
    sw $0, $2, 0
    lw $0, $1, 4
    beq $0, $2, 8
    beq $0, $5, 1
    beq $3, $2, 10
    j 1

    li $v0, 10
    syscall
