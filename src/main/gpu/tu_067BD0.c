#include "include_asm.h"

extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;

int VSync(int arg0);

typedef unsigned int u32;

asm(".globl func_80077548");
asm("func_80077548 = Gpu_QueryStatus");

extern volatile u32 *g_GpuGp0Ptr;
extern u32 *g_GpuGp1Ptr;

void Gpu_ResetDmaWaitTimer(void) {
    g_GpuDmaTimeoutDeadline = VSync(-1) + 0xF0;
    g_GpuDmaWaitLoopCounter = 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_067BD0", Gpu_DmaTimeoutCheck);

int Gpu_QueryStatus(int arg0) {
    u32 status;

    *g_GpuGp1Ptr = 0x10000007;
    status = *g_GpuGp0Ptr & 0xFFFFFF;
    if (status != 2) {
        *g_GpuGp0Ptr = (*g_GpuGp1Ptr & 0x3FFF) | 0xE1001000;
        (void)*g_GpuGp0Ptr;
        return 0;
    }
    if ((arg0 & 8) == 0) {
        return 1;
    }
    *g_GpuGp1Ptr = 0x09000001;
    return 2;
}
