/* Readable PSX candidate: NOT a byte match. No EABI required.
 * Only the global return-address and BIOS selector bindings remain.
 * See psx_minimal_pins.md for compiler results and validation limits.
 */
register void *returnAddress asm("$31");
typedef unsigned Word;
typedef Word *(*BiosTableQuery)(void);
struct BiosC0Table {
    Word reserved[6];
    Word *handler;
};
extern void *savedReturnAddress;
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void FlushCache(void);
extern const Word templateStart[];
void _ExitCard(void) {
    register Word selector asm("$9");
    const Word *source;
    Word *handler;
    struct BiosC0Table *table;
    savedReturnAddress = returnAddress;
    EnterCriticalSection();
    selector = 86;
    asm volatile("" : : "r"(selector));
    table = (struct BiosC0Table *)((BiosTableQuery)0xb0)();
    handler = table->handler;
    for (source = templateStart; source != templateStart + 3; ++source, ++handler)
        *(volatile Word *)(handler + 28) = *source;
    FlushCache();
    ExitCriticalSection();
    returnAddress = savedReturnAddress;
}
