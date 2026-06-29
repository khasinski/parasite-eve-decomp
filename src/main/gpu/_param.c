typedef unsigned long u_long;

extern volatile u_long *g_GpuGp0Ptr;
extern volatile u_long *g_GpuGp1Ptr;

int _param(int x) {
    *g_GpuGp1Ptr = x | 0x10000000;
    return *g_GpuGp0Ptr & 0xFFFFFF;
}
