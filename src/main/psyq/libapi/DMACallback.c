typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void DMACallback(void) {
    g_EventCallbackTable[1]();
}
