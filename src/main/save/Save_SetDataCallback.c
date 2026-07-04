/* MASPSX_FLAGS: --store-return-delay */
extern int g_CdReadCompleteCallback;

int Save_SetDataCallback(int arg0) {
    int old = g_CdReadCompleteCallback;
    g_CdReadCompleteCallback = arg0;
    return old;
}
