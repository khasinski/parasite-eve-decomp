/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/gpu_dma_state.h"

extern u32 *g_GpuGp1Ptr;
extern u32 *g_GpuDmaChcrPtr;
extern u32 *g_GpuDmaControlRegPtr;
extern u32 g_GpuDmaQueueHead;
extern volatile u32 g_GpuDmaQueueTail;
extern u32 D_80095884;
extern u32 D_80095874;
extern volatile u32 D_80095878;
extern u32 *D_80095860, *D_80095854;
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

void Gpu_ResetDmaWaitTimer(void);
int Gpu_SetDisplayBuffer(void);
int Gpu_DmaTimeoutCheck(void);

static __inline__ unsigned int readGpuStatus(void) {
    unsigned int status = *D_80095854;
    asm volatile("" : "+r"(status));
    return status;
}

int Gpu_DrainDmaQueue(int mode) {
    int queued;
    if (mode == 0) {
        Gpu_ResetDmaWaitTimer();
        goto checkQueue;
    retryQueue:
        Gpu_SetDisplayBuffer();
        if (Gpu_DmaTimeoutCheck()) return -1;
    checkQueue:
        if (D_80095874 != D_80095878) goto retryQueue;
        goto checkHardware;
    retryHardware:
        if (Gpu_DmaTimeoutCheck()) return -1;
    checkHardware:
        if (*D_80095860 & 0x01000000) goto retryHardware;
        if (!(readGpuStatus() & 0x04000000)) goto retryHardware;
        return 0;
    }
    queued = (D_80095874 - D_80095878) & 63;
    if (queued) Gpu_SetDisplayBuffer();
    if ((*D_80095860 & 0x01000000) || !(readGpuStatus() & 0x04000000)) {
        return queued ? queued : 1;
    }
    return queued;
}
