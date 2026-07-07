/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 */
/* MASPSX_FLAGS: --stack-return-delay */


#include "include_asm.h"
asm(".globl SetDispMask");
asm("SetDispMask = SetDispMask");

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, int arg0);
extern GpuCallbacks * g_GpuCallbacks[];
#define g_GpuCallbacks (g_GpuCallbacks[0])
extern char D_80011870[];
void GPU_memset(unsigned char *dst, int value, int count);

void SetDispMask(int arg0) {
    register int saved asm("$16");
    GpuCallbacks *callbacks;

    saved = arg0;
    if (g_GraphDebug[0] >= 2) {
        g_GpuDebugPrintf[0](D_80011870, saved);
    }

    if (saved) {
        goto after_memset;
    }

    GPU_memset(g_GraphDebug + 0x6A, -1, 0x14);

after_memset:
    callbacks = g_GpuCallbacks;
    callbacks->callback10(saved ? 0x3000000 : 0x3000001);
}
