extern void D_800A34B0;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void SysDeqIntRP(int index, void *queue);

int RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &D_800A34B0);
    ExitCriticalSection();
    return 1;
}
