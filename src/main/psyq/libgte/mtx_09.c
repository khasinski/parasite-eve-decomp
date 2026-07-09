void SetRotMatrix(int *m) {
    register int r0 asm("$8") = m[0];
    register int r1 asm("$9") = m[1];
    register int r2 asm("$10") = m[2];
    register int r3 asm("$11") = m[3];
    register int r4 asm("$12") = m[4];

    asm volatile("ctc2 %0,$0" : : "r"(r0));
    asm volatile("ctc2 %0,$1" : : "r"(r1));
    asm volatile("ctc2 %0,$2" : : "r"(r2));
    asm volatile("ctc2 %0,$3" : : "r"(r3));
    asm volatile("ctc2 %0,$4" : : "r"(r4));
}
