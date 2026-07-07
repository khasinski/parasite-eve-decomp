/* MASPSX_FLAGS: --stack-return-delay */

typedef signed short s16;

typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    u8 x;
    u8 pad1;
    u8 y;
    u8 pad3;
    s16 w;
    s16 h;
} TexWindow;

unsigned int Gpu_BuildDrawOffsetCmd(unsigned int arg0, unsigned int arg1) {
    arg1 &= 0x7FF;
    arg1 <<= 11;
    arg0 &= 0x7FF;
    arg0 |= 0xE5000000;
    return arg1 | arg0;
}

u32 Gpu_BuildTexWindowCmd(TexWindow *tw) {
    volatile int slots[4];
    int x;
    int y;
    int w;
    int h_raw;
    int h;
    register u32 command asm("$4");

    if (tw != 0) {
        x = tw->x >> 3;
        slots[0] = x;
        w = ((-tw->w) & 0xFF) >> 3;
        slots[2] = w;
        y = tw->y >> 3;
        slots[1] = y;
        y <<= 15;
        h_raw = tw->h;
        command = 0xE2000000;
        h = ((-h_raw) & 0xFF) >> 3;
        slots[3] = h;

        return y | ((x << 10) | command) | (h << 5) | w;
    }

    return 0;
}
