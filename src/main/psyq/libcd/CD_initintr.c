extern void Cd_SetIntrMask(void);
extern void InterruptCallback(int event, void (*callback)(void));
extern void ResetCallback(void);

extern int g_CdReadyCallback;
extern int g_CdSyncCallback;
extern int g_CdResultByte;
extern int g_CdStatus;

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    g_CdResultByte = 0;
    /* Match note: ResetCallback has g_CdStatus clear in the call delay slot. */
    asm volatile(
        ".set\tnoreorder\n\t"
        ".set\tnoat\n\t"
        "lui\t$at,%%hi(g_CdStatus)\n\t"
        "jal\tResetCallback\n\t"
        "sw\t$zero,%%lo(g_CdStatus)($at)\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        :
        :
        : "$31", "memory");
    InterruptCallback(2, Cd_SetIntrMask);
}
