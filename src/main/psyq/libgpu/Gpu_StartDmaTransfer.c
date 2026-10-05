extern unsigned int *g_GpuGp1Ptr;
extern unsigned int *g_GpuDmaMadrPtr;
extern unsigned int *g_GpuDmaBcrPtr;
extern unsigned int *g_GpuDmaChcrPtr;

void Gpu_StartDmaTransfer(unsigned int value) {
    *g_GpuGp1Ptr = 0x4000002;
    *g_GpuDmaMadrPtr = value;
    *g_GpuDmaBcrPtr = 0;
    *g_GpuDmaChcrPtr = 0x1000401;
}
