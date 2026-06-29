/* MASPSX_FLAGS: --store-return-delay */
extern unsigned int g_DsSyncCallback;

void CdRom_SetSyncCallback(unsigned int value) {
    g_DsSyncCallback = value;
}
