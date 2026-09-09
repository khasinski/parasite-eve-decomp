#include "pe1/memcard.h"

extern int g_McOpPending;

extern void (*g_MemCardDelayedCallback)(void);
extern volatile int g_MemCardDelayedCallbackTimer;

int MemCard_IsPortTransferState(int arg0) {
    u8 value = g_MemCardPortStates[arg0].managerState;

    return (value == 3) || (value == 8) || (value == 10);
}

int MemCard_IsOperationPending(void) {
    return g_McOpPending;
}

void MemCard_ClearDelayedCallback(void) {
    g_MemCardDelayedCallback = 0;
    g_MemCardDelayedCallbackTimer = 0;
}
