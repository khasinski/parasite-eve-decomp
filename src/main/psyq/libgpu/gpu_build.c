/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ part: private draw-offset and texture-window command
 * builders and _status.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options or conflicting
 * declarations.
 */
#include "common.h"
#include "pe1/psyq_types.h"

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
    u32 command;
    u32 result;

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
        h = ((-h_raw) & 0xFF) >> 3;
        g_GpuTexWindowWork->slots[3] = h;

        command = (x << 10) | 0xE2000000;
        result = y | command | (h << 5) | w;
    }

    g_GpuTexWindowWork++;
    return result;
}

extern volatile u_long *g_GpuGp1Ptr;

u_long _status(void) {
    return *g_GpuGp1Ptr;
}
