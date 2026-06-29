extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}
