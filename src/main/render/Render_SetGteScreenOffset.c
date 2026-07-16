extern unsigned short D_800BCF94;
extern unsigned short D_800BCF96;

int Render_SetGteScreenOffset(void) {
    register int x asm("v1") = D_800BCF94;
    register int y asm("a0") = D_800BCF96;
    register int sx asm("t4");
    register int sy asm("t5");


    sx = x << 16;
    sy = y << 16;
    asm volatile("ctc2 %0,$24" : : "r"(sx));
    asm volatile("ctc2 %0,$25" : : "r"(sy));
    return 0;
}
