extern void (*g_IntrVSyncCallbackTable[])(void);

void setIntrVSync(unsigned int index, void (*callback)(void)) {
    if (callback != g_IntrVSyncCallbackTable[index]) {
        g_IntrVSyncCallbackTable[index] = callback;
    }
}

void memclrIntrVSync(int *ptr, int count) {
    int i = count - 1;

    if (count != 0) {
        do {
            *ptr = 0;
            i--;
            ptr++;
        } while (i != -1);
    }
}
