/* MASPSX_FLAGS: --store-return-delay */

extern unsigned int *g_GpuGp1Ptr;
extern unsigned char g_GpuControlRegMirror[];

void Gpu_WriteControlAndMirror(unsigned int value) {
    *g_GpuGp1Ptr = value;
    g_GpuControlRegMirror[value >> 24] = value;
}
