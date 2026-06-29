void SetTransMatrix(void *m) {
    asm("lw $8,20($4)");
    asm("lw $9,24($4)");
    asm("lw $10,28($4)");
    asm("ctc2 $8,$5");
    asm("ctc2 $9,$6");
    asm("ctc2 $10,$7");
}
