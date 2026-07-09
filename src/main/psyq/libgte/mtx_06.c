void *ApplyMatrixSV(int *m, void *v, short *out) {
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];
    register int ir1 asm("$8");
    register int ir2 asm("$9");
    register int ir3 asm("$10");
    register void *ret asm("$2");

    asm volatile("ctc2 %0,$0" : : "r"(r0));
    asm volatile("ctc2 %0,$1" : : "r"(r1));
    asm volatile("ctc2 %0,$2" : : "r"(r2));
    asm volatile("ctc2 %0,$3" : : "r"(r3));
    asm volatile("ctc2 %0,$4" : : "r"(r4));
    asm volatile("lwc2 $0,0(%0)" : : "r"(v));
    asm volatile("lwc2 $1,4(%0)" : : "r"(v));
    asm volatile("nop");
    asm volatile(".word 0x4A486012");
    asm volatile("mfc2 %0,$9" : "=r"(ir1));
    asm volatile("mfc2 %0,$10" : "=r"(ir2));
    asm volatile("mfc2 %0,$11" : "=r"(ir3));
    out[0] = ir1;
    out[1] = ir2;
    out[2] = ir3;
    asm volatile("" ::: "memory");
    asm volatile("" : "=r"(ret) : "0"(out));
    return ret;
}
