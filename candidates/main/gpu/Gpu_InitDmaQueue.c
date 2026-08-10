/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned int u32;

extern volatile u32 *g_GpuGp1Ptr;
extern volatile u32 *g_GpuDmaChcrPtr;
extern volatile u32 *g_GpuDmaControlRegPtr;
extern u32 g_GpuDmaQueueHead;
extern volatile u32 g_GpuDmaQueueTail;
extern u32 D_80095884;
extern unsigned char D_800A3348[];
extern unsigned char D_800BD030[];

int SetIntrMask(int mask);
void GPU_memset(unsigned char *dst, int value, int count);
int Gpu_QueryStatus(int arg0);

int Gpu_InitDmaQueue(int mode) {
    int saved_mask;
    int channel;

    saved_mask = SetIntrMask(0);
    g_GpuDmaQueueTail = 0;
    D_80095884 = saved_mask;
    g_GpuDmaQueueHead = g_GpuDmaQueueTail;

    channel = mode & 7;
    switch (channel) {
    case 0:
    case 5:
        *g_GpuDmaChcrPtr = 0x401;
        *g_GpuDmaControlRegPtr |= 0x800;
        *g_GpuGp1Ptr = 0;
        GPU_memset(D_800A3348, 0, 0x100);
        GPU_memset(D_800BD030, 0, 0x1800);
        break;

    case 1:
    case 3:
        *g_GpuDmaChcrPtr = 0x401;
        *g_GpuDmaControlRegPtr |= 0x800;
        *g_GpuGp1Ptr = 0x02000000;
        *g_GpuGp1Ptr = 0x01000000;
        break;
    }

    SetIntrMask(D_80095884);

    if ((mode & 7) == 0) {
        return Gpu_QueryStatus(mode);
    }

    return 0;
}
