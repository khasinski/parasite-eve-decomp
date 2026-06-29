extern void (*g_IntrVSyncCallbackTable[])(void);

void setIntrVSync(unsigned int index, void (*callback)(void)) {
    if (callback != g_IntrVSyncCallbackTable[index]) {
        g_IntrVSyncCallbackTable[index] = callback;
    }
}
