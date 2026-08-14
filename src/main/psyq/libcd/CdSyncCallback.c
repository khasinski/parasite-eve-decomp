extern int g_CdSyncCallback;

int CdSyncCallback(int callback) {
    int old;

    old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}
