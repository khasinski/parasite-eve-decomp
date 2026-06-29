void *ApplyMatrix(void *m, void *in, void *out) {
    asm("lw $8,0($4)");
    asm("lw $9,4($4)");
    asm("lw $10,8($4)");
    asm("lw $11,12($4)");
    asm("lw $12,16($4)");
    asm("ctc2 $8,$0");
    asm("ctc2 $9,$1");
    asm("ctc2 $10,$2");
    asm("ctc2 $11,$3");
    asm("ctc2 $12,$4");
    asm("lwc2 $0,0($5)");
    asm("lwc2 $1,4($5)");
    asm("nop");
    asm(".word 0x4A486012");
    asm("swc2 $25,0($6)");
    asm("swc2 $26,4($6)");
    asm("swc2 $27,8($6)");
    asm("addu $2,$6,$0");
}
