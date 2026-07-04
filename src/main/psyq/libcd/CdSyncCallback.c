/* MASPSX_FLAGS: --store-return-delay */
extern int g_CdSyncCallback;

int CdSyncCallback(int callback) {
    int old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}
