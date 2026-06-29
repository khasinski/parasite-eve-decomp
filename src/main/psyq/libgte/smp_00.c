void LightColor(void *in, void *out) {
    asm("lwc2 $9,0($4)");
    asm("lwc2 $10,4($4)");
    asm("lwc2 $11,8($4)");
    asm("nop");
    asm(".word 0x4A4DA412");
    asm("swc2 $9,0($5)");
    asm("swc2 $10,4($5)");
    asm("swc2 $11,8($5)");
}

void DpqColorLight(void *in, void *rgb, int p, void *out) {
    asm("lwc2 $9,0($4)");
    asm("lwc2 $10,4($4)");
    asm("lwc2 $11,8($4)");
    asm("lwc2 $6,0($5)");
    asm("mtc2 $6,$8");
    asm("nop");
    asm(".word 0x4A680029");
    asm("swc2 $22,0($7)");
}

void DpqColor3(void *c0, void *c1, void *c2, int p, void *out0, void *out1, void *out2) {
    asm("lwc2 $20,0($4)");
    asm("lwc2 $21,0($5)");
    asm("lwc2 $22,0($6)");
    asm("lwc2 $6,0($6)");
    asm("mtc2 $7,$8");
    asm("nop");
    asm(".word 0x4AF8002A");
    asm("lw $8,0x10($29)");
    asm("lw $9,0x14($29)");
    asm("lw $10,0x18($29)");
    asm("swc2 $20,0($8)");
    asm("swc2 $21,0($9)");
    asm("swc2 $22,0($10)");
}

void Intpl(void *in, int p, void *out) {
    asm("lwc2 $9,0($4)");
    asm("lwc2 $10,4($4)");
    asm("lwc2 $11,8($4)");
    asm("mtc2 $5,$8");
    asm("nop");
    asm(".word 0x4A980011");
    asm("swc2 $22,0($6)");
}

asm(".text");
asm(".set noreorder");
asm(".globl Square12");
asm("Square12:");
asm("lwc2 $9,0($4)");
asm("lwc2 $10,4($4)");
asm("lwc2 $11,8($4)");
asm("nop");
asm(".word 0x4AA80428");
asm("swc2 $25,0($5)");
asm("swc2 $26,4($5)");
asm("swc2 $27,8($5)");
asm("jr $31");
asm("addu $2,$5,$0");

asm(".globl Square0");
asm("Square0:");
asm("lwc2 $9,0($4)");
asm("lwc2 $10,4($4)");
asm("lwc2 $11,8($4)");
asm("nop");
asm(".word 0x4AA00428");
asm("swc2 $25,0($5)");
asm("swc2 $26,4($5)");
asm("swc2 $27,8($5)");
asm("jr $31");
asm("addu $2,$5,$0");
asm(".set reorder");

int AverageZ3(int sz1, int sz2, int sz3) {
    int ret;

    asm("mtc2 $4,$17");
    asm("mtc2 $5,$18");
    asm("mtc2 $6,$19");
    asm("nop");
    asm(".word 0x4B58002D");
    asm("mfc2 $2,$7");
    return ret;
}

int AverageZ4(int sz0, int sz1, int sz2, int sz3) {
    int ret;

    asm("mtc2 $4,$16");
    asm("mtc2 $5,$17");
    asm("mtc2 $6,$18");
    asm("mtc2 $7,$19");
    asm("nop");
    asm(".word 0x4B68002E");
    asm("mfc2 $2,$7");
    return ret;
}

void OuterProduct12(void *a, void *b, void *out) {
    asm("cfc2 $13,$0");
    asm("cfc2 $14,$2");
    asm("cfc2 $15,$4");
    asm("lw $8,0($4)");
    asm("lw $9,4($4)");
    asm("lw $10,8($4)");
    asm("ctc2 $8,$0");
    asm("ctc2 $9,$2");
    asm("ctc2 $10,$4");
    asm("lwc2 $11,8($5)");
    asm("lwc2 $9,0($5)");
    asm("lwc2 $10,4($5)");
    asm("nop");
    asm(".word 0x4B78000C");
    asm("swc2 $25,0($6)");
    asm("swc2 $26,4($6)");
    asm("swc2 $27,8($6)");
    asm("ctc2 $13,$0");
    asm("ctc2 $14,$2");
    asm("ctc2 $15,$4");
}

void OuterProduct0(void *a, void *b, void *out) {
    asm("cfc2 $13,$0");
    asm("cfc2 $14,$2");
    asm("cfc2 $15,$4");
    asm("lw $8,0($4)");
    asm("lw $9,4($4)");
    asm("lw $10,8($4)");
    asm("ctc2 $8,$0");
    asm("ctc2 $9,$2");
    asm("ctc2 $10,$4");
    asm("lwc2 $11,8($5)");
    asm("lwc2 $9,0($5)");
    asm("lwc2 $10,4($5)");
    asm("nop");
    asm(".word 0x4B70000C");
    asm("swc2 $25,0($6)");
    asm("swc2 $26,4($6)");
    asm("swc2 $27,8($6)");
    asm("ctc2 $13,$0");
    asm("ctc2 $14,$2");
    asm("ctc2 $15,$4");
}

int Lzc(int value) {
    int ret;

    asm("mtc2 $4,$30");
    asm("nop");
    asm("nop");
    asm("mfc2 $2,$31");
    return ret;
}
