typedef void (*EventCallback)(int, int);

extern EventCallback *g_EventCallbackTable;

void VSyncCallback(int arg0) {
    g_EventCallbackTable[5](4, arg0);
}
