#ifndef PE1_GPU_DMA_STATE_H
#define PE1_GPU_DMA_STATE_H

typedef unsigned int u32;

/*
 * GPU DMA controller state at 0x80095850.  The register pointers precede the
 * circular queue and its interrupt/timeout bookkeeping in memory.
 */
typedef struct GpuDmaState {
    /* 0x00 */ volatile u32 *gp0;
    /* 0x04 */ u32 *gp1;
    /* 0x08 */ volatile u32 *madr;
    /* 0x0C */ volatile u32 *bcr;
    /* 0x10 */ volatile u32 *chcr;
    /* 0x14 */ u32 reserved14;
    /* 0x18 */ u32 reserved18;
    /* 0x1C */ u32 reserved1C;
    /* 0x20 */ volatile u32 *control;
    /* 0x24 */ u32 queueHead;
    /* 0x28 */ volatile u32 queueTail;
    /* 0x2C */ u32 reserved2C;
    /* 0x30 */ u32 reserved30;
    /* 0x34 */ u32 interruptMask;
    /* 0x38 */ int timeoutDeadline;
    /* 0x3C */ int waitLoopCounter;
} GpuDmaState;

/* 64 entries at 0x800BD030; the callback receives the two stored words. */
typedef struct GpuQueueEntry {
    /* 0x00 */ void (*function)(u32, u32);
    /* 0x04 */ u32 argument0;
    /* 0x08 */ u32 argument1;
    /* 0x0C */ u32 packet[21];
} GpuQueueEntry;

extern GpuQueueEntry D_800BD030[64];
extern GpuDmaState D_80095850;

#endif /* PE1_GPU_DMA_STATE_H */
