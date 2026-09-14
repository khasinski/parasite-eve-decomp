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
    volatile unsigned *hook;
    unsigned i;

    EnterCriticalSection();
    hook = GetC0Table()[C0_EXCEPTION_HANDLER]
         + CARD_IRQ_HOOK_OFFSET / sizeof(unsigned);

    for (i = 0; i < CARD_IRQ_HOOK_WORDS; ++i)
        hook[i] = cardIrqDisabledPatch[i];

    FlushCache();
    ExitCriticalSection();
}
