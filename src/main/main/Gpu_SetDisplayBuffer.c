/* ASSEMBLER: GNU */
/* GAS_VERSION: 2.7 */
#include "pe1/gpu_state.h"
#include "pe1/gpu_dma_state.h"

extern u32 *D_80095860;
extern u32 *D_80095854;
extern volatile u32 D_80095874;
extern volatile u32 D_80095878;
extern u32 D_80095880;
int SetIntrMask(int);

/* LIBGPU queue drain (_exeque); the existing project name is retained.
 * Entries have a callback, two arguments, and an 84-byte packet payload.
 * Empty barriers and six register pins preserve the retail scheduling and
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
                register u32 arg1Offset asm("$2");
                register u32 arg1;
                register void (*function)(u32, u32) asm("$2");

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
        register u32 mask asm("$3");
        asm volatile("" : "=r"(status) : "0"(status));
        mask = 0x01000000;
        if (!(status & mask)) {
            register u32 *pending = &D_8009574C.syncCallbackPending;
            asm volatile("" : "=r"(pending) : "0"(pending));
            if (*pending) {
                register void (*callback)(void) asm("$4") = *(void (**)(void))(pending + 1);
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
