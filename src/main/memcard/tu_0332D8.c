typedef unsigned char u8;

extern u8 D_800A0ED5[];

extern int g_McOpPending;

extern volatile int g_MemCardDelayedCallback;
extern volatile int g_MemCardDelayedCallbackTimer;

int MemCard_IsPortTransferState(int arg0) {
    u8 value = D_800A0ED5[arg0 * 0x418];

    return (value == 3) || (value == 8) || (value == 10);
}

int MemCard_IsOperationPending(void) {
    return g_McOpPending;
}

void MemCard_ClearDelayedCallback(void) {
    g_MemCardDelayedCallback = 0;
    g_MemCardDelayedCallbackTimer = 0;
}
