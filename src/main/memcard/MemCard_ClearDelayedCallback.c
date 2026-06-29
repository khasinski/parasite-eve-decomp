extern volatile int g_MemCardDelayedCallback;
extern volatile int g_MemCardDelayedCallbackTimer;

void MemCard_ClearDelayedCallback(void) {
    g_MemCardDelayedCallback = 0;
    g_MemCardDelayedCallbackTimer = 0;
}
