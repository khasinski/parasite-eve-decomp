/* PSY-Q LIBPAD PADMAIN, part 5 of 8: _padStopCom. */
extern void D_800A5AB0;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void ChangeClearRCnt(int counter, int mode);
void SysDeqIntRP(int index, void *queue);

void _padStopCom(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &D_800A5AB0);
    ExitCriticalSection();
}
