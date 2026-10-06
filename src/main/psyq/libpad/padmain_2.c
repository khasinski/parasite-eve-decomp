/* PSY-Q LIBPAD PADMAIN, part 2 of 8: _padSetVsyncParam. */
extern void MemCard_TimerCallback(void);
int MemCard_TimerReadyCallback(void);
extern void *D_800A5AB4[];

void _padSetVsyncParam(void) {
    void **table;

    table = D_800A5AB4;
    asm volatile("" : "=r"(table) : "0"(table));
    table[0] = MemCard_TimerCallback;
    table[1] = MemCard_TimerReadyCallback;
    table[-1] = 0;
    table[2] = 0;
}
