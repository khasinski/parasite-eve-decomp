static int Render_SetScreenOffsetRegs(int x, int y) {
    x <<= 16;
    y <<= 16;
    asm volatile("ctc2 %0,$24" : : "r"(x));
    asm volatile("ctc2 %0,$25" : : "r"(y));
    return 0;
}

int Render_ResetGteScreenOffset(void) {
    return Render_SetScreenOffsetRegs(0xA0, 0x70);
}
