extern int EnterCriticalSection(void);
extern int ExitCriticalSection(void);
extern int SysDeqIntRP(int, void *);
extern int D_800A34B0;

int RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &D_800A34B0);
    ExitCriticalSection();
    return 1;
}
