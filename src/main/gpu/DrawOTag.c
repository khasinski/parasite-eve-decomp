/* MASPSX_FLAGS: --stack-return-delay */

asm(".globl func_800753B4");
asm("func_800753B4 = DrawOTag");

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *name, int arg0);
extern GpuCallbacks * g_GpuCallbacks;
extern char D_80011928[];

int DrawOTag(int arg0) {
    GpuCallbacks *callbacks;

    if (g_GraphDebug[0] >= 2) {
        g_GpuDebugPrintf[0](D_80011928, arg0);
    }

    callbacks = g_GpuCallbacks;
    return callbacks->addque2(callbacks->u18.packet, arg0, 0, 0);
}
