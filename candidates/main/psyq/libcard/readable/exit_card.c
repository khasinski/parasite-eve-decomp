/* CC1_FLAGS: -fno-schedule-insns -fno-schedule-insns2 */
/* Semantic reconstruction of LIBCARD/END; not yet a byte match.
 * GetC0Table is an explicit BIOS API dependency, not a hidden ASM macro.
 */
enum {
    C0_EXCEPTION_HANDLER = 6,
    CARD_IRQ_HOOK_OFFSET = 0x70,
    CARD_IRQ_HOOK_WORDS = 3
};

extern unsigned *const *GetC0Table(void);
extern void EnterCriticalSection(void);
extern void FlushCache(void);
extern void ExitCriticalSection(void);

/* Retail stores this three-NOP replacement at 0x8007E584. */
extern const unsigned cardIrqDisabledPatch[CARD_IRQ_HOOK_WORDS];

void _ExitCard(void)
{
    volatile unsigned *handler;
    const unsigned *source;

    EnterCriticalSection();
    handler = GetC0Table()[C0_EXCEPTION_HANDLER];

    for (source = cardIrqDisabledPatch;
         source != cardIrqDisabledPatch + CARD_IRQ_HOOK_WORDS;
         ++source, ++handler)
        handler[CARD_IRQ_HOOK_OFFSET / sizeof(unsigned)] = *source;

    FlushCache();
    ExitCriticalSection();
}
