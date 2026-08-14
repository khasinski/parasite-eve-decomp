/* CC1_VERSION: 2.8.1 */
extern int g_CdSyncCallback;

int CdSyncCallback(int callback) {
    int old;

    old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}
