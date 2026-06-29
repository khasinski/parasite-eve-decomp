#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);

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
