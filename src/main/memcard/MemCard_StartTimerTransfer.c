extern void MemCard_StartCounterIrq(void);

void MemCard_StartTimerTransfer(void) {
    MemCard_StartCounterIrq();
}
