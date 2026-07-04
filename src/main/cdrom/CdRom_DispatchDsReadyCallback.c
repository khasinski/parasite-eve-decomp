extern void (*g_LibDsReadyCallback)(int);

void CdRom_DispatchDsReadyCallback(int arg0);

void CdRom_DispatchDsReadyCallback(int arg0) {
    if (g_LibDsReadyCallback != 0) {
        g_LibDsReadyCallback((unsigned char)arg0);
    }
}
