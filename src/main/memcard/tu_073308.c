/* MASPSX_FLAGS: --store-return-delay */
#include "include_asm.h"

#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);

extern int g_MemCardCallbackPending;

int MemCard_TimerReadyCallback(void) {
    MemCardState *state = g_MemCardState;

    if ((state->field4 & 1) == 0) {
        return 0;
    }
    if ((state->field0 & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_073308", MemCard_TimerCallback);

int MemCard_TakeCallback(void) {
    int old = g_MemCardCallbackPending;
    g_MemCardCallbackPending = 0;
    return old;
}
