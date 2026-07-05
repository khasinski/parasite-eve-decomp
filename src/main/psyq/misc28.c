typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void ResetCallback(void) {
    g_EventCallbackTable[3]();
}

void InterruptCallback(void) {
    g_EventCallbackTable[2]();
}

void DMACallback(void) {
    g_EventCallbackTable[1]();
}
