void SetBackColor(int r, int g, int b) {
    r <<= 4;
    g <<= 4;
    b <<= 4;
    asm volatile("ctc2 %0,$13" : : "r"(r));
    asm volatile("ctc2 %0,$14" : : "r"(g));
    asm volatile("ctc2 %0,$15" : : "r"(b));
}
