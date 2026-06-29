extern void MemCard_StopCounterIrq(void);

void MemCard_StopTimerTransfer(void) {
    MemCard_StopCounterIrq();
}
