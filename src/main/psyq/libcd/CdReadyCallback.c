/* MASPSX_FLAGS: --store-return-delay */
extern int g_CdReadyCallback;

int CdReadyCallback(int callback) {
    int old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}
