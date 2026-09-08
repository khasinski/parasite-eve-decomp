/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"
extern char D_800119BC[];
void checkRECT(char *, RECT *);
extern volatile unsigned int *g_GpuDmaChcrPtr;
extern unsigned int *g_GpuGp1Ptr;
extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;
int VSync(int mode);
int Gpu_DmaTimeoutCheck(void);
void Gpu_RestoreDmaCallback(void);
void DMACallback(int channel, void (*callback)(void));

int LoadImage2(RECT *rect, unsigned int *data)
{
    register unsigned int status asm("$2");
    checkRECT(D_800119BC, rect);
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
    D_80095744->u20.load(rect, data);
    return 0;
}
