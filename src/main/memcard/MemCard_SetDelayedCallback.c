extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

void MemCard_SetDelayedCallback(void (*callback)(void)) {
    g_MemCardDelayedCallback = callback;
    g_MemCardDelayedCallbackTimer = 1;
}
