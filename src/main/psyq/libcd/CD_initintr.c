/* MASPSX_FLAGS: --store-call-delay */

extern void Cd_SetIntrMask(void);
extern void InterruptCallback(int event, void (*callback)(void));
extern void ResetCallback(void);

extern int g_CdReadyCallback;
extern int g_CdSyncCallback;
extern int g_CdResultByte;
extern int g_CdStatus;

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    g_CdResultByte = 0;
    g_CdStatus = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);
}
