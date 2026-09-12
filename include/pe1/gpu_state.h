#ifndef PE1_GPU_STATE_H
#define PE1_GPU_STATE_H

#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_callbacks.h"

/*
 * libgpu/sys.c state immediately following the GPU dispatch and debug-print
 * callbacks.  The byte offsets are observed by SetGraph*, checkRECT, and
 * DrawSyncCallback; the DRAWENV cache starts immediately after this record.
 */
typedef struct GpuQueueState {
    /* 0x00 */ unsigned char queue;
    /* 0x01 */ unsigned char debugLevel;
} GpuQueueState;

typedef struct GpuDebugState {
    /* 0x00 */ unsigned char type;
    /* 0x01 */ GpuQueueState queueState;
    /* 0x03 */ unsigned char reverse;
    /* 0x04 */ short width;
    /* 0x06 */ short height;
    /* 0x08 */ unsigned int syncCallbackPending;
    /* 0x0C */ void (*drawSyncCallback)();
} GpuDebugState;

extern GpuCallbacks *D_80095744;
extern void (*D_80095748)();
extern GpuDebugState D_8009574C;
extern char D_80011840[];

#endif /* PE1_GPU_STATE_H */
