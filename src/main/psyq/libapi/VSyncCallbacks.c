typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void VSyncCallbacks(void) {
    g_EventCallbackTable[5]();
}
