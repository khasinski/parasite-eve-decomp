void SetLightMatrix(void *m) {
    asm("lw $8,0($4)");
    asm("lw $9,4($4)");
    asm("lw $10,8($4)");
    asm("lw $11,12($4)");
    asm("lw $12,16($4)");
    asm("ctc2 $8,$8");
    asm("ctc2 $9,$9");
    asm("ctc2 $10,$10");
    asm("ctc2 $11,$11");
    asm("ctc2 $12,$12");
}
