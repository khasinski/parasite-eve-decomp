/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

typedef unsigned char u8;
typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern char D_800118C8[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int ClearImage2(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118C8, rect);
    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->clr, rect, 8, 0x80000000 | (b << 16) | (g << 8) | r);
}
