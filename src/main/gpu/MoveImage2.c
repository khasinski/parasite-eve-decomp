/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern volatile u32 *g_GpuGp1Ptr;
extern volatile u32 *g_GpuDmaChcrPtr;
extern int g_GpuDmaTimeoutDeadline;
extern int g_GpuDmaWaitLoopCounter;
extern GpuCallbacks *g_GpuCallbacks;
extern u32 D_800957EC[];
extern char D_800118EC[];

void checkRECT(char *name, RECT *rect);
int VSync(int arg0);
int Gpu_DmaTimeoutCheck(void);
void Gpu_RestoreDmaCallback(void);
void DMACallback(int arg0, void (*arg1)(void));

int MoveImage2(RECT *rect, int x, int y) {
    u32 *packet;
    register int dst asm("$2");
    u32 xy;
    register u32 status asm("$2");
    volatile u32 *dma;
    u32 command;
    register u32 low asm("$3");
    u32 wh;

    checkRECT(D_800118EC, rect);
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

    if (rect->w == 0) {
        return -1;
    }
    if (rect->h != 0) {
        register GpuCallbacks *callbacks asm("$3");

        dst = y << 16;
        low = x & 0xFFFF;
        command = dst | low;
        xy = *(u32 *)&rect->x;
        callbacks = g_GpuCallbacks;
        asm volatile("" : "=r"(xy), "=r"(callbacks) : "0"(xy), "1"(callbacks));
        asm volatile("" : "=r"(packet) : "0"(D_800957EC), "r"(command), "r"(xy), "r"(callbacks));
        packet[1] = command;
        packet[0] = xy;
        wh = *(u32 *)&rect->w;
        packet[2] = wh;
        callbacks->u18.moveImage(packet - 2);
        return 0;
    }

    return -1;
}
