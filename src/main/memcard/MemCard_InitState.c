extern unsigned char g_MemCardPortStates[];
extern int D_800A12F8;
extern int D_800A0EE0;
extern int g_MemCardServicedPort;
extern int g_MemCardInfoPollCountdown;
extern int g_MemCardConnectDebounce;
extern int D_800A184C;
extern int D_800A1848;
extern int g_McOpPending;
extern int g_MemCardActiveState;
extern int g_MemCardActivePortOneBased;
extern int g_MemCardSavePollTimeout;
extern int g_MemCardActivePromptPending;
extern int g_SaveTitleStyleFlag;
extern int g_MemCardLoadSucceeded;
extern int g_MemCardReadContext;

void bzero(void *ptr, int size);

void MemCard_InitState(void) {
    bzero(g_MemCardPortStates, 0x830);
    D_800A12F8 = -1;
    D_800A0EE0 = -1;
    g_MemCardServicedPort = 0;
    g_MemCardInfoPollCountdown = 0;
    g_MemCardConnectDebounce = 0;
    D_800A184C = 0;
    D_800A1848 = 0;
    g_McOpPending = 0;
    g_MemCardActiveState = 0;
    g_MemCardActivePortOneBased = 0;
    g_MemCardSavePollTimeout = 0;
    g_MemCardActivePromptPending = 0;
    g_SaveTitleStyleFlag = 0;
    g_MemCardLoadSucceeded = 0;
    g_MemCardReadContext = 0;
}
