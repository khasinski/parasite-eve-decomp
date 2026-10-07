#include "pe1/gpu_dma_state.h"

typedef unsigned long u_long;

extern volatile u_long *g_GpuGp0Ptr;
extern volatile u_long *g_GpuGp1Ptr;


int _param(int x) {
    *g_GpuGp1Ptr = x | 0x10000000;
    return *g_GpuGp0Ptr & 0xFFFFFF;
}

void Gpu_SubmitPacket(GpuQueueFunction function, u32 *source, u32 argument) {
    Gpu_SwapDisplayBuffers(function, source, 0, argument);
}
