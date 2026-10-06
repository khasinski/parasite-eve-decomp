/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ part: private command queue submission, queue
 * callback, initialization and drain.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options or conflicting
 * declarations.
 */
#include "pe1/gpu_state.h"
#include "pe1/gpu_dma_state.h"

extern volatile u32 D_80095874, D_80095878;
extern u32 D_8009587C;
extern u32 *D_80095860, *D_80095854;
void Gpu_ResetDmaWaitTimer(void);
int Gpu_DmaTimeoutCheck(void);
int Gpu_SetDisplayBuffer(void);
int SetIntrMask(int);

/* LIBGPU command submission: execute immediately when possible, otherwise
 * copy an optional packet into the ring and publish its callback/arguments.
 * The existing project symbol name is retained. Empty ordering barriers
 * are tracked in debt. */
int Gpu_SwapDisplayBuffers(void (*function)(u32, u32), u32 *source, int size, u32 argument) {
    int i;
    GpuDebugState *state;

    Gpu_ResetDmaWaitTimer();
    while (((D_80095874 + 1) & 63) == D_80095878) {
        if (Gpu_DmaTimeoutCheck()) return -1;
        Gpu_SetDisplayBuffer();
    }
    D_8009587C = SetIntrMask(0);
    state = &D_8009574C;
    state->syncCallbackPending = 1;
    if (state->queueState.queue) {
        register u32 status;
        register u32 mask;
        if (D_80095874 != D_80095878) goto queued;
        status = *D_80095860;
        asm volatile("" : "=r"(status) : "0"(status));
        mask = 0x01000000;
        if (status & mask) goto queued;
        if (state->drawSyncCallback) goto queued;
    }
    {
        register volatile u32 *gp1 = (volatile u32 *)D_80095854;
        register u32 ready = 0x04000000;
        do {} while (!(*gp1 & ready));
        function((u32)source, argument);
        SetIntrMask(D_8009587C);
        return 0;
    }

queued:
    DMACallback(2, (PsyqInterruptHandler)Gpu_SetDisplayBuffer);
    i = 0;
    if (size) {
        register u32 *base = D_800BD030[0].packet;
        register u32 *cursor = source;
        for (; i < size / 4;) {
            register u32 offset = i * 4;
            register u32 value = *cursor++;
            register u32 head;
            u32 entry;
            head = D_80095874;
            i++;
            entry = head * sizeof(GpuQueueEntry);
            entry += (u32)base;
            offset += entry;
            *(u32 *)offset = value;
            asm volatile("" ::: "memory");
        }
        D_800BD030[D_80095874].argument0 = (u32)D_800BD030[D_80095874].packet;
    } else {
        D_800BD030[D_80095874].argument0 = (u32)source;
    }
    D_800BD030[D_80095874].argument1 = argument;
    asm volatile("" ::: "memory");
    D_800BD030[D_80095874].function = function;
    asm volatile("" ::: "memory");
    D_80095874 = (D_80095874 + 1) & 63;
    SetIntrMask(D_8009587C);
    Gpu_SetDisplayBuffer();
    return (D_80095874 - D_80095878) & 63;
}

extern u32 *D_80095860;
extern u32 *D_80095854;
extern volatile u32 D_80095874;
extern volatile u32 D_80095878;
extern u32 D_80095880;
int SetIntrMask(int);

/* LIBGPU queue drain (_exeque); the existing project name is retained.
 * Entries have a callback, two arguments, and an 84-byte packet payload.
 * Empty barriers and register pins preserve the retail scheduling and
 * interrupt-visible reads; they are included in the matching debt report. */
int Gpu_SetDisplayBuffer(void) {
    register u32 readyMask;
    register u32 busyMask;

    if (*D_80095860 & 0x01000000) return 1;
    D_80095880 = SetIntrMask(0);
    if (D_80095874 != D_80095878 && !(*D_80095860 & 0x01000000)) {
        readyMask = 0x04000000;
        busyMask = 0x01000000;
        do {
            if (((D_80095878 + 1) & 63) == D_80095874 &&
                D_8009574C.drawSyncCallback == 0) {
                DMACallback(2, 0);
            }
            {
                register volatile u32 *gp1 = (volatile u32 *)D_80095854;
                if (!(*gp1 & readyMask)) {
                    register u32 ready = 0x04000000;
                    do {} while (!(*gp1 & ready));
                }
            }
            {
                register u32 fnIndex asm("$5");
                register u32 argIndex;
                register u32 argOffset asm("$2");
                register u32 fnOffset;
                register u32 arg0;
                register u32 arg1Index;
                u32 arg1Offset;
                register u32 arg1;
                void (*function)(u32, u32);
                /* Keep the three separate reads of the interrupt-visible tail. */
                fnIndex = D_80095878;
                argIndex = D_80095878;
                argOffset = argIndex * sizeof(GpuQueueEntry);
                fnOffset = fnIndex * 3;
                arg0 = ((GpuQueueEntry *)((char *)D_800BD030 + argOffset))->argument0;
                asm volatile("" : "=r"(arg0), "=r"(fnOffset)
                             : "0"(arg0), "1"(fnOffset) : "memory");
                arg1Index = D_80095878;
                fnOffset <<= 5;
                arg1Offset = arg1Index * sizeof(GpuQueueEntry);
                arg1 = ((GpuQueueEntry *)((char *)D_800BD030 + arg1Offset))->argument1;
                function = ((GpuQueueEntry *)((char *)D_800BD030 + fnOffset))->function;
                function(arg0, arg1);
            }
            D_80095878 = (D_80095878 + 1) & 63;
        } while (D_80095874 != D_80095878 && !(*D_80095860 & busyMask));
    }
    SetIntrMask(D_80095880);
    if (D_80095874 == D_80095878) {
        register u32 status = *D_80095860;
        u32 mask;
        asm volatile("" : "=r"(status) : "0"(status));
        mask = 0x01000000;
        if (!(status & mask)) {
            register u32 *pending = &D_8009574C.syncCallbackPending;
                        if (*pending) {
                void (*callback)(void) = *(void (**)(void))(pending + 1);
                if (callback) {
                    register GpuDebugState *base = (GpuDebugState *)((char *)pending - 8);
                    asm volatile("" : "=r"(base) : "0"(base));
                    base->syncCallbackPending = 0;
                    asm volatile("" ::: "memory");
                    callback();
                }
            }
        }
    }
    return (D_80095874 - D_80095878) & 63;
}

extern u32 *g_GpuGp1Ptr;
extern u32 *g_GpuDmaChcrPtr;
extern u32 *g_GpuDmaControlRegPtr;
extern volatile u32 g_GpuDmaQueueHead;
extern volatile u32 g_GpuDmaQueueTail;
extern u32 D_80095884;
extern u32 D_80095874;
extern volatile u32 D_80095878;
extern u32 *D_80095860, *D_80095854;
extern unsigned char D_800A3348[];

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
        GPU_memset((unsigned char *)D_800BD030, 0, 0x1800);
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
