/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/gpu_state.h"

int SetGraphQueue(int mode)
{
    register GpuQueueState *state asm("$17") = &D_8009574C.queueState;
    int previous = state->queue;
    if (state->debugLevel >= 2) {
        D_80095748(D_80011840, mode);
    }
    if (mode != state->queue) {
        D_80095744->reset(1);
        state->queue = mode;
        DMACallback(2, 0);
    }
    return previous;
}
