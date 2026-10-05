typedef unsigned long u_long;

extern volatile u_long *g_GpuGp0Ptr;
extern volatile u_long *g_GpuGp1Ptr;

void Gpu_SwapDisplayBuffers(int arg0, int arg1, int arg2, int arg3);

int _param(int x) {
    *g_GpuGp1Ptr = x | 0x10000000;
    return *g_GpuGp0Ptr & 0xFFFFFF;
}

void Gpu_SubmitPacket(int arg0, int arg1, int arg2) {
    Gpu_SwapDisplayBuffers(arg0, arg1, 0, arg2);
}
