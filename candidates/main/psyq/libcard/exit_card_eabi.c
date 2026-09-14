/* EABI comparison reference, not a production PSX candidate.
 * Requires stock GCC 2.95.2 with -mabi=eabi.
 * CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2
 * Six register bindings and two empty barriers remain. See exit_card_eabi.md.
 */
register void *returnAddress asm("$31");
extern void *savedReturnAddress;
extern void EnterCriticalSection(void);
extern void FlushCache(void);
extern void ExitCriticalSection(void);
extern unsigned templateStart[], templateEnd[];
void _ExitCard(void) {
    register unsigned selector asm("$9");
    register unsigned *(*bios)(void) asm("$10");
    unsigned *kernel;
    register unsigned *source asm("$10");
    register unsigned *end asm("$9");
    register unsigned word asm("$3");
    savedReturnAddress = returnAddress;
    EnterCriticalSection();
    selector = 0x56;
    asm volatile("" : : "r"(selector));
    bios = (unsigned *(*)(void))0xb0;
    asm volatile("" : "=r"(bios) : "0"(bios));
    kernel = bios();
    kernel = (unsigned *)kernel[6];
    source = templateStart;
    end = templateEnd;
    for (;;) {
        word = *(volatile unsigned *)source;
        *(volatile unsigned *)(kernel + 28) = word;
        ++source;
        ++kernel;
        if (source == end)
            break;
    }
    FlushCache();
    ExitCriticalSection();
    returnAddress = savedReturnAddress;
}
