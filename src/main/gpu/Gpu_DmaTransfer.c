/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug;
extern void (*g_GpuDebugPrintf)(char *name, void *arg0);
extern volatile u32 *g_GpuGp1Ptr;
extern volatile u32 *g_GpuDmaChcrPtr;
extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;
extern GpuCallbacks *g_GpuCallbacks;
extern char D_80011928[];

int VSync(int arg0);
int Gpu_DmaTimeoutCheck(void);
void Gpu_RestoreDmaCallback(void);
void DMACallback(int arg0, void (*arg1)(void));

int Gpu_DmaTransfer(void *arg0) {
    register u32 status asm("$2");
    volatile u32 *dma;

    if (g_GraphDebug >= 2) {
        g_GpuDebugPrintf(D_80011928, arg0);
    }

    g_GpuDmaTimeoutDeadline = VSync(-1) + 0xF0;
    g_GpuDmaWaitLoopCounter = 0;

    dma = g_GpuDmaChcrPtr;
    status = *dma;
    goto check_dma;

wait_again:
    if (Gpu_DmaTimeoutCheck() != 0) {
        return -1;
    }
    status = *g_GpuDmaChcrPtr;

check_dma:
    if ((status & 0x01000000) != 0) {
        goto wait_again;
    }
    if ((*g_GpuGp1Ptr & 0x04000000) == 0) {
        goto wait_again;
    }

    DMACallback(2, Gpu_RestoreDmaCallback);
    g_GpuCallbacks->u18.moveImage(arg0);
    return 0;
}
