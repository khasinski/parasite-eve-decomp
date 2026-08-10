#include "common.h"
#include "pe1/memcard.h"
extern int g_MemCardActivePortOneBased;
extern int g_MemCardActivePromptPending;

void MemCard_StartRead(int port, int arg1);

extern u8 D_800A0EDE[];

extern int g_MemCardReadContext;

void MenuWidget_SaveAndSetCurrentNode(int arg0);

void MemCard_StartActivePortRead(void) {
    MemCard_StartRead(g_MemCardActivePortOneBased - 1, 1);
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}

int MemCard_GetPortFileCount(int arg0) {
    return D_800A0EDE[arg0 * 0x418];
}

void MemCard_StartRead(int port, int arg1) {
    MemCardPortState *state = &g_MemCardPortStates[port];

    if (state->managerState == 0 || state->managerState == 12) {
        state->managerState = 1;
        state->nextState = 2;
        state->retryCount = 10;
        MenuWidget_SaveAndSetCurrentNode(0);
        g_MemCardReadContext = arg1;
    }
}
