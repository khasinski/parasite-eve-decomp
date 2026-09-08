extern void (*g_IntrVSyncCallbackTable[])(void);

void setIntrVSync(unsigned int index, void (*callback)(void)) {
    if (callback != g_IntrVSyncCallbackTable[index]) {
        g_IntrVSyncCallbackTable[index] = callback;
    }
}

void memclrIntrVSync(void *ptr, int count) {
    int *words = ptr;
    int i = count - 1;

    if (count != 0) {
        do {
            *words = 0;
            i--;
            words++;
        } while (i != -1);
    }
}
