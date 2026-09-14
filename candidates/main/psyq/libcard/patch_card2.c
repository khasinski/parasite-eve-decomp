/* PsyQ 4.3 LIBCARD/PATCH: _patch_card2 (retail func_8007E470).
 * Requires stock native GCC 2.95.2, mipstx39el-unknown-elf, EABI.
 * See patch_card2.md; this candidate is not in the production manifest.
 */
register void *returnAddress asm("$31");
extern void *savedReturnAddress;
extern void EnterCriticalSection(void);
extern void FlushCache(void);
extern unsigned templateStart[], templateEnd[];
void _patch_card2(void) {
    register unsigned selector asm("$9");
    register unsigned *(*bios)(void) asm("$10");
    unsigned *kernel;
    register unsigned *source asm("$10");
    register unsigned *end asm("$9");
    register unsigned word asm("$8");
    register unsigned previous asm("$3");
    savedReturnAddress = returnAddress;
    EnterCriticalSection();
    selector = 0x57;
    asm volatile("" : : "r"(selector));
    bios = (unsigned *(*)(void))0xb0;
    asm volatile("" : "=r"(bios) : "0"(bios));
    kernel = bios();
    kernel = (unsigned *)kernel[91];
    /* Preserve the original read of the first instruction being patched. */
    previous = *(volatile unsigned *)(kernel + 626);
    asm volatile("" : : "r"(previous));
    source = templateStart;
    end = templateEnd;
    do {
        word = *(volatile unsigned *)source;
        *(volatile unsigned *)(kernel + 626) = word;
        asm volatile("" : "=r"(source) : "0"(source));
        ++source;
        ++kernel;
    } while (source != end);
    /* The original leaves the critical section active for its caller. */
    FlushCache();
    returnAddress = savedReturnAddress;
}
