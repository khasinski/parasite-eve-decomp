typedef void (*EventCallback)(void);

extern EventCallback *g_EventCallbackTable;

void RestartCallback(void) {
    g_EventCallbackTable[6]();
}
