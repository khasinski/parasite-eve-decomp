/* MASPSX_FLAGS: --stack-return-delay */

asm("DrawPrim = DrawPrim");

typedef unsigned char u8;

typedef int (*Cb3C)(int);
typedef int (*Cb14)(void *, u8);

#include "pe1/gpu_callbacks.h"

extern GpuCallbacks *g_GpuCallbacks;

int DrawPrim(void *arg0) {
    u8 *sp;
    u8 value;

    sp = (u8 *)arg0;
    value = sp[3];
    g_GpuCallbacks->callback3C(0);
    return g_GpuCallbacks->cb14(sp + 4, value);
}
