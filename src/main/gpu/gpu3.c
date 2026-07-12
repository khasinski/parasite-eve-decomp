#include "include_asm.h"

extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;

int VSync(int arg0);
int printf(char *fmt, ...);
int SetIntrMask(int mask);

typedef unsigned int u32;

extern volatile u32 *g_GpuGp0Ptr;
extern u32 *g_GpuGp1Ptr;
extern volatile u32 *g_GpuDmaMadrPtr;
extern volatile u32 *g_GpuDmaChcrPtr;
extern volatile u32 *g_GpuDmaControlRegPtr;
extern u32 g_GpuDmaQueueHead;
extern volatile u32 g_GpuDmaQueueTail;
extern u32 D_80095884;
extern char D_80011988[];

void Gpu_ResetDmaWaitTimer(void) {
    g_GpuDmaTimeoutDeadline = VSync(-1) + 0xF0;
    g_GpuDmaWaitLoopCounter = 0;
}

int Gpu_DmaTimeoutCheck(void) {
    int now;
    register int count asm("$2");
    register int old_count asm("$4");
    int *counter;
    int mask;
    volatile u32 *ptr;
    register volatile u32 *gp1 asm("$6");
    register u32 head asm("$5");

    now = VSync(-1);
    if (g_GpuDmaTimeoutDeadline >= now) {
        counter = &g_GpuDmaWaitLoopCounter;
        count = *counter;
        old_count = count;
        count = count + 1;
        *counter = count;
        if (old_count <= 0xF0000) {
            goto return_zero;
        }
    }

    gp1 = g_GpuGp1Ptr;
    asm volatile("" : "=r"(gp1) : "0"(gp1));
    (void)*gp1;
    head = g_GpuDmaQueueHead;

    printf(D_80011988,
           (head - g_GpuDmaQueueTail) & 0x3F,
           *gp1,
           *g_GpuDmaChcrPtr,
           *g_GpuDmaMadrPtr);

    mask = SetIntrMask(0);
    g_GpuDmaQueueTail = 0;
    D_80095884 = mask;
    g_GpuDmaQueueHead = g_GpuDmaQueueTail;

    *g_GpuDmaChcrPtr = 0x401;
    ptr = g_GpuDmaControlRegPtr;
    *ptr = *ptr | 0x800;
    *g_GpuGp1Ptr = 0x02000000;
    *g_GpuGp1Ptr = 0x01000000;
    SetIntrMask(D_80095884);
    return -1;

return_zero:
    return 0;
}

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
