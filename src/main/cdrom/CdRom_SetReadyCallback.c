/* MASPSX_FLAGS: --store-return-delay */
extern unsigned int g_DsReadyCallback;

void CdRom_SetReadyCallback(unsigned int value) {
    g_DsReadyCallback = value;
}
