void *TransMatrix(void *m, void *v) {
    asm("lw $8,0($5)");
    asm("lw $9,4($5)");
    asm("lw $10,8($5)");
    asm("sw $8,20($4)");
    asm("sw $9,24($4)");
    asm("sw $10,28($4)");
    asm("addu $2,$4,$0");
}
