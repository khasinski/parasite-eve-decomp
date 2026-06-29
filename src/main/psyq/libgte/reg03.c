void SetVertex0(void *v) {
    asm("lwc2 $0,0($4)");
    asm("lwc2 $1,4($4)");
}

void SetVertex1(void *v) {
    asm("lwc2 $2,0($4)");
    asm("lwc2 $3,4($4)");
}

void SetVertex2(void *v) {
    asm("lwc2 $4,0($4)");
    asm("lwc2 $5,4($4)");
}

void SetVertexTri(void *v0, void *v1, void *v2) {
    asm("lwc2 $0,0($4)");
    asm("lwc2 $1,4($4)");
    asm("lwc2 $2,0($5)");
    asm("lwc2 $3,4($5)");
    asm("lwc2 $4,0($6)");
    asm("lwc2 $5,4($6)");
}

void SetRGBfifo(void *c0, void *c1, void *c2) {
    asm("lwc2 $20,0($4)");
    asm("lwc2 $21,0($5)");
    asm("lwc2 $22,0($6)");
}

void SetIR123(int ir1, int ir2, int ir3) {
    asm("mtc2 $4,$9");
    asm("mtc2 $5,$10");
    asm("mtc2 $6,$11");
}

void SetIR0(int ir0) {
    asm("mtc2 $4,$8");
}

void SetSZfifo3(int sz1, int sz2, int sz3) {
    asm("mtc2 $4,$17");
    asm("mtc2 $5,$18");
    asm("mtc2 $6,$19");
}

void SetSZfifo4(int sz0, int sz1, int sz2, int sz3) {
    asm("mtc2 $4,$16");
    asm("mtc2 $5,$17");
    asm("mtc2 $6,$18");
    asm("mtc2 $7,$19");
}

void SetSXSYfifo(int sxy0, int sxy1, int sxy2) {
    asm("mtc2 $4,$12");
    asm("mtc2 $5,$13");
    asm("mtc2 $6,$14");
}

void SetRii(int r11, int r22, int r33) {
    asm("ctc2 $4,$0");
    asm("ctc2 $5,$2");
    asm("ctc2 $6,$4");
}

void SetMAC123(int mac1, int mac2, int mac3) {
    asm("mtc2 $4,$25");
    asm("mtc2 $5,$26");
    asm("mtc2 $6,$27");
}

void SetData32(int value) {
    asm("mtc2 $4,$30");
}

void SetDQA(int value) {
    asm("ctc2 $4,$27");
}

void SetDQB(int value) {
    asm("ctc2 $4,$28");
}
