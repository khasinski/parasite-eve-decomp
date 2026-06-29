typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void StopCallback(void) {
    g_EventCallbackTable[4]();
}
