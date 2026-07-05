/* MASPSX_FLAGS: --store-return-delay */

extern unsigned int g_DsPollCallback;

extern unsigned int g_DsSyncCallback;

extern unsigned int g_DsReadyCallback;

extern unsigned int g_DsReadDoneCallback;

void CdRom_SetPollCallback(unsigned int value) {
    g_DsPollCallback = value;
}

void CdRom_SetSyncCallback(unsigned int value) {
    g_DsSyncCallback = value;
}

void CdRom_SetReadyCallback(unsigned int value) {
    g_DsReadyCallback = value;
}

void CdRom_SetDispatchCallback(unsigned int value) {
    g_DsReadDoneCallback = value;
}
