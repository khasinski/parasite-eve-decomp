extern int EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void DsDataCallback(void *callback);
extern void DsSyncCallback(void *callback);
extern void CdDataCallback(void *callback);
extern void CdReadyCallback(void *callback);

extern int D_8009AFD8;
extern unsigned char *D_8009AF1C;
extern unsigned char *D_8009AF28;

void CdRom_Shutdown(void) {
    unsigned char *ptr;

    EnterCriticalSection();
    if (D_8009AFD8 == 1) {
        DsDataCallback(0);
        DsSyncCallback(0);
    } else {
        CdDataCallback(0);
        CdReadyCallback(0);
    }
    *D_8009AF1C = 0;
    ptr = D_8009AF28;
    asm volatile("nop");
    *ptr = 0;
    ExitCriticalSection();
}
