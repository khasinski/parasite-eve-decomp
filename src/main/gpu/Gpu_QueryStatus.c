typedef unsigned int u32;

asm(".globl func_80077548");
asm("func_80077548 = Gpu_QueryStatus");

extern volatile u32 *g_GpuGp0Ptr;
extern u32 *g_GpuGp1Ptr;

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
