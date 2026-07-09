void SetTransMatrix(int *m) {
    register int tx asm("$8") = m[5];
    register int ty asm("$9") = m[6];
    register int tz asm("$10") = m[7];

    asm volatile("ctc2 %0,$5" : : "r"(tx));
    asm volatile("ctc2 %0,$6" : : "r"(ty));
    asm volatile("ctc2 %0,$7" : : "r"(tz));
}
