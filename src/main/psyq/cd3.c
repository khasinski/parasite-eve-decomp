/* MASPSX_FLAGS: --store-return-delay */

void CD_flush(void);

extern int g_CdReadCallback;

void CdFlush(void) {
    CD_flush();
}

int CdReadCallback(int callback) {
    int old = g_CdReadCallback;
    g_CdReadCallback = callback;
    return old;
}
