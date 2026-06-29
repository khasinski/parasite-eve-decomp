/* MASPSX_FLAGS: --stack-return-delay */
extern int g_VSyncCount;
extern void (*g_IntrVSyncCallbackTable[])(void);

void trapIntrVSync(void) {
    int i;

    g_VSyncCount++;
    for (i = 0; i < 8; i++) {
        if (g_IntrVSyncCallbackTable[i] != 0) {
            g_IntrVSyncCallbackTable[i]();
        }
    }
}
