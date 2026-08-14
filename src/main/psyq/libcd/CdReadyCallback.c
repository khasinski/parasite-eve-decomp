/* AS_MODE: reorder */
extern int g_CdReadyCallback;

int CdReadyCallback(int callback) {
    int old;

    old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}
