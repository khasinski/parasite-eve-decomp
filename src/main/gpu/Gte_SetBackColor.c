typedef unsigned short u16;

void Gte_SetBackColor(u16 *dst, int r, int g, int b) {
    int i;

    r <<= 4;
    g <<= 4;
    b <<= 4;
    asm volatile("ctc2 %0,$13" : : "r"(r));
    asm volatile("ctc2 %0,$14" : : "r"(g));
    asm volatile("ctc2 %0,$15" : : "r"(b));

    for (i = 0; i < 9; i++) {
        dst[i] = 0;
    }
}
