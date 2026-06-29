extern volatile unsigned short *g_MemCardSioRegs;

void MemCard_WaitStatusBit2(void) {
    volatile unsigned short *ptr = g_MemCardSioRegs;

    /* Keep the poll loop target on the lhu, not the load-delay nop. */
    asm volatile("nop");
    while ((ptr[2] & 2) == 0) {
    }
}
