typedef unsigned long u_long;

extern volatile u_long *g_GpuGp1Ptr;

u_long _status(void) {
    return *g_GpuGp1Ptr;
}
