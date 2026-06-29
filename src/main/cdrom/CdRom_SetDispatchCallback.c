/* MASPSX_FLAGS: --store-return-delay */
extern unsigned int g_DsReadDoneCallback;

void CdRom_SetDispatchCallback(unsigned int value) {
    g_DsReadDoneCallback = value;
}
