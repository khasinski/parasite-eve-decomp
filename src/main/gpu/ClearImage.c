/* MASPSX_FLAGS: --stack-return-delay */

typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} RECT;

#include "pe1/gpu_callbacks.h"

extern char D_800118BC[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118BC, rect);
    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->clr, rect, 8, (b << 16) | (g << 8) | r);
}
