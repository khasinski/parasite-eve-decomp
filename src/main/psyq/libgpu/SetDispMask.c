/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "include_asm.h"
#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, int arg0);
extern GpuCallbacks *g_GpuCallbacks;
extern char D_80011870[];
void GPU_memset(unsigned char *dst, int value, int count);

void SetDispMask(int mask) {
    GpuCallbacks *callbacks;

    if (g_GraphDebug[0] >= 2) {
        g_GpuDebugPrintf[0](D_80011870, mask);
    }

    if (mask == 0) {
        GPU_memset(g_GraphDebug + 0x6A, -1, 0x14);
    }

    callbacks = g_GpuCallbacks;
    callbacks->callback10(mask ? 0x3000000 : 0x3000001);
}
