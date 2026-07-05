typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

extern unsigned short D_800945E6;

extern unsigned short *g_IntrMaskRegPtr;

void VSyncCallbacks(void) {
    g_EventCallbackTable[5]();
}

void StopCallback(void) {
    g_EventCallbackTable[4]();
}

void RestartCallback(void) {
    g_EventCallbackTable[6]();
}

unsigned short CheckCallback(void) {
    return D_800945E6;
}

unsigned short GetIntrMask(void) {
    return *g_IntrMaskRegPtr;
}
