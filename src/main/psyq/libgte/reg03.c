void SetVertex0(void *v) {
    asm volatile("lwc2 $0,0(%0)" : : "r"(v));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v));
}

void SetVertex1(void *v) {
    asm volatile("lwc2 $2,0(%0)" : : "r"(v));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v));
}

void SetVertex2(void *v) {
    asm volatile("lwc2 $4,0(%0)" : : "r"(v));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v));
}

void SetVertexTri(void *v0, void *v1, void *v2) {
    asm volatile("lwc2 $0,0(%0)" : : "r"(v0));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v0));
    asm volatile("lwc2 $2,0(%0)" : : "r"(v1));
    asm volatile("lwc2 $3,4(%0)" : : "r"(v1));
    asm volatile("lwc2 $4,0(%0)" : : "r"(v2));
    asm volatile("lwc2 $5,4(%0)" : : "r"(v2));
}

void SetRGBfifo(void *c0, void *c1, void *c2) {
    asm volatile("lwc2 $20,0(%0)" : : "r"(c0));
    asm volatile("lwc2 $21,0(%0)" : : "r"(c1));
    asm volatile("lwc2 $22,0(%0)" : : "r"(c2));
}

void SetIR123(int ir1, int ir2, int ir3) {
    asm volatile("mtc2 %0,$9" : : "r"(ir1));
    asm volatile("mtc2 %0,$10" : : "r"(ir2));
    asm volatile("mtc2 %0,$11" : : "r"(ir3));
}

void SetIR0(int ir0) {
    asm volatile("mtc2 %0,$8" : : "r"(ir0));
}

void SetSZfifo3(int sz1, int sz2, int sz3) {
    asm volatile("mtc2 %0,$17" : : "r"(sz1));
    asm volatile("mtc2 %0,$18" : : "r"(sz2));
    asm volatile("mtc2 %0,$19" : : "r"(sz3));
}

void SetSZfifo4(int sz0, int sz1, int sz2, int sz3) {
    asm volatile("mtc2 %0,$16" : : "r"(sz0));
    asm volatile("mtc2 %0,$17" : : "r"(sz1));
    asm volatile("mtc2 %0,$18" : : "r"(sz2));
    asm volatile("mtc2 %0,$19" : : "r"(sz3));
}

void SetSXSYfifo(int sxy0, int sxy1, int sxy2) {
    asm volatile("mtc2 %0,$12" : : "r"(sxy0));
    asm volatile("mtc2 %0,$13" : : "r"(sxy1));
    asm volatile("mtc2 %0,$14" : : "r"(sxy2));
}

void SetRii(int r11r12, int r22r23, int r33) {
    asm volatile("ctc2 %0,$0" : : "r"(r11r12));
    asm volatile("ctc2 %0,$2" : : "r"(r22r23));
    asm volatile("ctc2 %0,$4" : : "r"(r33));
}

void SetMAC123(int mac1, int mac2, int mac3) {
    asm volatile("mtc2 %0,$25" : : "r"(mac1));
    asm volatile("mtc2 %0,$26" : : "r"(mac2));
    asm volatile("mtc2 %0,$27" : : "r"(mac3));
}

void SetData32(int data) {
    asm volatile("mtc2 %0,$30" : : "r"(data));
}

void SetDQA(int dqa) {
    asm volatile("ctc2 %0,$27" : : "r"(dqa));
}

void SetDQB(int dqb) {
    asm volatile("ctc2 %0,$28" : : "r"(dqb));
}
