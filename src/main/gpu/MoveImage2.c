/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
extern char D_800118EC[];
extern unsigned int D_800957EC[3];
void checkRECT(char *, RECT *);
extern volatile unsigned int *g_GpuDmaChcrPtr;
extern unsigned int *g_GpuGp1Ptr;
extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;
extern GpuCallbacks *D_80095744;
int VSync(int mode);
int Gpu_DmaTimeoutCheck(void);
void Gpu_RestoreDmaCallback(void);
void DMACallback(int channel, void (*callback)(void));

int MoveImage2(RECT *rect, int x, int y)
{
    register unsigned int status asm("$2");
    checkRECT(D_800118EC, rect);
    g_GpuDmaTimeoutDeadline = VSync(-1) + 240;
    g_GpuDmaWaitLoopCounter = 0;
    status = *g_GpuDmaChcrPtr;
    goto check;
retry:
    if (Gpu_DmaTimeoutCheck()) return -1;
    status = *g_GpuDmaChcrPtr;
check:
    if (status & 0x01000000) goto retry;
    /* The GPU-ready test is reached only after DMA is idle. */
    asm volatile("" : : : "memory");
    if (!(*g_GpuGp1Ptr & 0x04000000)) goto retry;
    DMACallback(2, Gpu_RestoreDmaCallback);
    if (!rect->w || !rect->h) return -1;
    D_800957EC[0] = *(unsigned int *)&rect->x;
    D_800957EC[1] = ((unsigned int)y << 16) | (x & 0xFFFF);
    D_800957EC[2] = *(unsigned int *)&rect->w;
    D_80095744->u18.moveImage((char *)D_800957EC - 8);
    return 0;
}
