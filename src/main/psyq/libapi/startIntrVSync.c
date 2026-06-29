extern int g_IntrVSyncCallbackTable[];
extern int g_VSyncCount;
extern int *D_800956B0;

void memclrIntrVSync(int *ptr, int count);
void trapIntrVSync(void);
void InterruptCallback(int event, void (*callback)(void));
void setIntrVSync(void);

void *startIntrVSync(void) {
    *D_800956B0 = 0x107;
    g_VSyncCount = 0;
    memclrIntrVSync(g_IntrVSyncCallbackTable, 8);
    InterruptCallback(0, trapIntrVSync);
    return setIntrVSync;
}
