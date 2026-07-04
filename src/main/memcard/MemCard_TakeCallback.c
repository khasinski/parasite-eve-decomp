/* MASPSX_FLAGS: --store-return-delay */
extern int g_MemCardCallbackPending;

int MemCard_TakeCallback(void) {
    int old = g_MemCardCallbackPending;
    g_MemCardCallbackPending = 0;
    return old;
}
