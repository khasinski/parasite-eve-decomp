extern void (*g_LibDsReadyCallback)(int);

asm(".globl CdRom_DispatchDsReadyCallback");
asm("CdRom_DispatchDsReadyCallback = func_8007F960");

void CdRom_DispatchDsReadyCallback(int arg0) __asm__("func_8007F960");

void CdRom_DispatchDsReadyCallback(int arg0) {
    if (g_LibDsReadyCallback != 0) {
        g_LibDsReadyCallback((unsigned char)arg0);
    }
}
