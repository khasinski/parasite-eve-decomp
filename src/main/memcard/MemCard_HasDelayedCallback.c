extern int g_MemCardDelayedCallback;

int MemCard_HasDelayedCallback(void) {
    return g_MemCardDelayedCallback != 0;
}
