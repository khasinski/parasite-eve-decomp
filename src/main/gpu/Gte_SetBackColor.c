typedef unsigned short u16;

void Gte_SetBackColor(u16 *dst, int r, int g, int b) {
    register int sr asm("t4");
    register int sg asm("t5");
    register int sb asm("t6");

    sr = r << 4;
    sg = g << 4;
    sb = b << 4;
    asm volatile("ctc2 %0,$13" : : "r"(sr));
    asm volatile("ctc2 %0,$14" : : "r"(sg));
    asm volatile("ctc2 %0,$15" : : "r"(sb));

    dst[0] = 0;
    dst[1] = 0;
    dst[2] = 0;
    dst[3] = 0;
    dst[4] = 0;
    dst[5] = 0;
    dst[6] = 0;
    dst[7] = 0;
    dst[8] = 0;
}
