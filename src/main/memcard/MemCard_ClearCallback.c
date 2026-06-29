extern void MemCard_TakeCallback(void);

void MemCard_ClearCallback(void) {
    MemCard_TakeCallback();
}
