/* MASPSX_FLAGS: --store-return-delay */

extern int g_CdSyncCallback;

extern int g_CdReadyCallback;

int CdSyncCallback(int callback) {
    int old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}

int CdReadyCallback(int callback) {
    int old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}
