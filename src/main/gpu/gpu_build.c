#include "common.h"

typedef struct {
    u8 x;
    u8 pad1;
    u8 y;
    u8 pad3;
    s16 w;
    s16 h;
} TexWindow;

typedef struct GpuTexWindowWork {
    volatile int slots[4];
} GpuTexWindowWork;

register GpuTexWindowWork *g_GpuTexWindowWork asm("$29");

unsigned int Gpu_BuildDrawOffsetCmd(unsigned int arg0, unsigned int arg1) {
    arg1 &= 0x7FF;
    arg1 <<= 11;
    arg0 &= 0x7FF;
    arg0 |= 0xE5000000;
    return arg1 | arg0;
}

u32 Gpu_BuildTexWindowCmd(TexWindow *tw) {
    int x;
    int y;
    int w;
    int h_raw;
    int h;
    register u32 command asm("$4");
    register u32 result asm("$2");

    g_GpuTexWindowWork--;
    if (tw == 0) {
        result = 0;
    } else {
        x = tw->x >> 3;
        g_GpuTexWindowWork->slots[0] = x;
        w = ((-tw->w) & 0xFF) >> 3;
        g_GpuTexWindowWork->slots[2] = w;
        y = tw->y >> 3;
        g_GpuTexWindowWork->slots[1] = y;
        y <<= 15;
        h_raw = tw->h;
        command = 0xE2000000;
        h = ((-h_raw) & 0xFF) >> 3;
        g_GpuTexWindowWork->slots[3] = h;

        result = y | ((x << 10) | command) | (h << 5) | w;
    }

    g_GpuTexWindowWork++;
    return result;
}
