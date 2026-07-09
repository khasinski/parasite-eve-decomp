void SetFarColor(int r, int g, int b) {
    r <<= 4;
    g <<= 4;
    b <<= 4;
    asm volatile("ctc2 %0,$21" : : "r"(r));
    asm volatile("ctc2 %0,$22" : : "r"(g));
    asm volatile("ctc2 %0,$23" : : "r"(b));
}
