/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
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
 * The existing project symbol name is retained. One register pin and empty
 * ordering barriers are tracked in debt. */
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
            register u32 entry asm("$2");
            head = D_80095874;
            asm volatile("" : "=r"(head), "=r"(i) : "0"(head), "1"(i));
            i++;
            asm volatile("" : "=r"(head), "=r"(i) : "0"(head), "1"(i));
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
