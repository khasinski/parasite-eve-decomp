extern void CD_flush(void);
extern void VSyncCallbacks(int, int);

extern int g_DsReadSysEnabled;
extern int g_CdSyncCallback;
extern int g_CdReadyCallback;

void CdRom_ClearDsCallbacks(void);

void CdRom_ClearDsCallbacks(void) {
    /* Match note: g_DsReadSysEnabled clear is in the CD_flush delay slot. */
    asm volatile(
        ".set\tnoreorder\n\t"
        ".set\tnoat\n\t"
        "lui\t$at,%%hi(g_DsReadSysEnabled)\n\t"
        "jal\tCD_flush\n\t"
        "sw\t$zero,%%lo(g_DsReadSysEnabled)($at)\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        :
        :
        : "$31", "memory");
    g_CdSyncCallback = 0;
    g_CdReadyCallback = 0;
    VSyncCallbacks(0, 0);
}
