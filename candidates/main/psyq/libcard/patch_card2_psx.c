/* Readable PSX candidate: NOT a byte match. No EABI required.
 * Only the global return-address and BIOS selector bindings remain.
 * See psx_minimal_pins.md for compiler results and validation limits.
 */
register void *returnAddress asm("$31");
typedef unsigned Word;
typedef Word *(*BiosTableQuery)(void);
struct BiosB0Table {
    Word reserved[91];
    Word *patchHost;
};
extern void *savedReturnAddress;
extern void EnterCriticalSection(void);
extern void FlushCache(void);
extern const Word templateStart[];
void _patch_card2(void) {
    register Word selector asm("$9");
    const Word *source;
    Word *host;
    struct BiosB0Table *table;
    savedReturnAddress = returnAddress;
    EnterCriticalSection();
    selector = 87;
    asm volatile("" : : "r"(selector));
    table = (struct BiosB0Table *)((BiosTableQuery)0xb0)();
    host = table->patchHost;
    (void)*(volatile Word *)(host + 626);
    for (source = templateStart; source != templateStart + 5; ++source, ++host)
        *(volatile Word *)(host + 626) = *source;
    FlushCache();
    returnAddress = savedReturnAddress;
}
