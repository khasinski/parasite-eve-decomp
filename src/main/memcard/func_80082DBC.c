extern void D_800A5AB0;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void ChangeClearRCnt(int counter, int mode);
void SysDeqIntRP(int index, void *queue);

void func_80082DBC(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &D_800A5AB0);
    ExitCriticalSection();
}
