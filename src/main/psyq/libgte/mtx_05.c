void *ApplyMatrix(int *m, void *v, void *out) {
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];
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
    asm volatile("swc2 $25,0(%0)" : : "r"(out));
    asm volatile("swc2 $26,4(%0)" : : "r"(out));
    asm volatile("swc2 $27,8(%0)" : : "r"(out));
    asm volatile("" : "=r"(ret) : "0"(out));
    return ret;
}
