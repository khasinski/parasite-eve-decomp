int Render_ResetGteScreenOffset(void) {
    register int x asm("v1") = 0xA0;
    register int y asm("a0") = 0x70;
    register int sx asm("t4");
    register int sy asm("t5");

    asm volatile("" : "=r"(x), "=r"(y) : "0"(x), "1"(y));
    sx = x << 16;
    sy = y << 16;
    asm volatile("ctc2 %0,$24" : : "r"(sx));
    asm volatile("ctc2 %0,$25" : : "r"(sy));
    return 0;
}
