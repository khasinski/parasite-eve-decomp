/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_state.h"

extern char D_80011814[];

int SetGraphDebug(int debugLevel) {
    u8 *currentDebugLevel;
    register int result asm("$2");
    GpuDebugPrintf debugPrint;
    int currentLevel;
    int type;
    int reverse;
    int oldDebugLevel;

    currentDebugLevel = &D_8009574C.queueState.debugLevel;
    /* Preserve the shared base used for the adjacent GPU state bytes. */
    asm("" : "+r"(currentDebugLevel));
    oldDebugLevel = *currentDebugLevel;
    *currentDebugLevel = debugLevel;
    result = oldDebugLevel;

    if ((u8)debugLevel == 0) {
        return result;
    }

    debugPrint = D_80095748;
    /* Keep the callback load ahead of its arguments. */
    asm volatile("" : "+r"(debugPrint));
    currentLevel = currentDebugLevel[0];
    type = currentDebugLevel[-2];
    reverse = currentDebugLevel[1];
    /* Materialize the byte arguments before the format string address. */
    asm volatile("" : "+r"(currentLevel), "+r"(type), "+r"(reverse));
    debugPrint(D_80011814, currentLevel, type, reverse);
    result = oldDebugLevel;
    return result;
}
#include "pe1/gpu_state.h"

int SetGraphQueue(int mode)
{
    GpuQueueState *state = &D_8009574C.queueState;
    int previous = state->queue;
    if (state->debugLevel >= 2) {
        /* Keep the two prior-state paths: stock GCC merges the calls after
         * choosing the retail register lifetimes. Both log exactly once. */
        if (previous) {
            D_80095748(D_80011840, mode);
        } else {
            D_80095748(D_80011840, mode);
        }
    }
    if (mode != state->queue) {
        D_80095744->reset(1);
        state->queue = mode;
        DMACallback(2, 0);
    }
    return previous;
}
#include "pe1/gpu_state.h"

int GetGraphDebug(void) {
    return D_8009574C.queueState.debugLevel;
}

#include "common.h"
#include "pe1/gpu_state.h"

extern char D_80011854[];

void *DrawSyncCallback(void *callback) {
    void *previous;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011854, callback);
    }

    previous = (void *)D_8009574C.drawSyncCallback;
    D_8009574C.drawSyncCallback = (void (*)())callback;
    return previous;
}