/* Local proof: 112/112 linked retail bytes match with UNMODIFIED native
 * GCC 2.95.2 configured as mipstx39el-unknown-elf, flags:
 * -O2 -G0 -mips1 -mcpu=3000 -msoft-float -mno-abicalls -mabi=eabi
 * followed by unchanged MASPSX 2.56. The decomp.me compiler selected below
 * is the available PSX build, NOT that EABI build: do not expect 100% here.
 * This scratch preserves the matching C source and the original target.
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
    register unsigned *kernel asm("$2");
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
    asm volatile("" : "=r"(kernel) : "0"(kernel));
    source = templateStart;
    end = templateEnd;
    do {
        word = *(volatile unsigned *)source;
        *(volatile unsigned *)(kernel + 28) = word;
        asm volatile("" : "=r"(source) : "0"(source));
        ++source;
        ++kernel;
    } while (source != end);
    FlushCache();
    ExitCriticalSection();
    returnAddress = savedReturnAddress;
}
