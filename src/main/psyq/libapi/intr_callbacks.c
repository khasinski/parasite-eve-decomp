typedef void (*EventCallback)();

extern EventCallback *g_EventCallbackTable;
extern unsigned short D_800945E6;
extern unsigned short *g_IntrMaskRegPtr;

void ResetCallback(void) {
    g_EventCallbackTable[3]();
}

void InterruptCallback(void) {
    g_EventCallbackTable[2]();
}

void DMACallback(void) {
    g_EventCallbackTable[1]();
}

void VSyncCallback(int arg0) {
    g_EventCallbackTable[5](4, arg0);
}

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

unsigned short SetIntrMask(unsigned short mask) {
    unsigned short prev = *g_IntrMaskRegPtr;
    *g_IntrMaskRegPtr = mask;
    return prev;
}
