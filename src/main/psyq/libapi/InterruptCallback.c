typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void InterruptCallback(void) {
    g_EventCallbackTable[2]();
}
