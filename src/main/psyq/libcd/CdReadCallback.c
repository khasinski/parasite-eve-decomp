/* MASPSX_FLAGS: --store-return-delay */
extern int g_CdReadCallback;

int CdReadCallback(int callback) {
    int old = g_CdReadCallback;
    g_CdReadCallback = callback;
    return old;
}
