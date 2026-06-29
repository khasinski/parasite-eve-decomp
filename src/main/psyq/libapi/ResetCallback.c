typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void ResetCallback(void) {
    g_EventCallbackTable[3]();
}
