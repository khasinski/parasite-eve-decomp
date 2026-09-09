void SetGeomOffset(int x, int y) {
    x <<= 16;
    y <<= 16;
    asm volatile("ctc2 %0,$24" : : "r"(x));
    asm volatile("ctc2 %0,$25" : : "r"(y));
}

static unsigned int tail_alignment_6981C[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
