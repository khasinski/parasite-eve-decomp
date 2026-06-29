void SetColorMatrix(void *m) {
    asm("lw $8,0($4)");
    asm("lw $9,4($4)");
    asm("lw $10,8($4)");
    asm("lw $11,12($4)");
    asm("lw $12,16($4)");
    asm("ctc2 $8,$16");
    asm("ctc2 $9,$17");
    asm("ctc2 $10,$18");
    asm("ctc2 $11,$19");
    asm("ctc2 $12,$20");
}
