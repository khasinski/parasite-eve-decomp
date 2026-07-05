extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

void MemCard_SetDelayedCallback(void (*callback)(void)) {
    g_MemCardDelayedCallback = callback;
    g_MemCardDelayedCallbackTimer = 1;
}

void MemCard_DelayedCallback(void) {
    void (*callback)(void);

    callback = g_MemCardDelayedCallback;
    if (callback != 0) {
        g_MemCardDelayedCallbackTimer++;
        if (g_MemCardDelayedCallbackTimer == 4) {
            callback();
            g_MemCardDelayedCallback = 0;
            g_MemCardDelayedCallbackTimer = 0;
        }
    }
}
