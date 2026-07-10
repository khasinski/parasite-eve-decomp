typedef unsigned int u32;

extern volatile u32 g_DsPollCallback;
extern volatile u32 g_DsSyncCallback;
extern volatile u32 g_DsReadyCallback;
extern u32 g_CdSyncCallback;
extern u32 g_CdReadyCallback;
extern u32 g_DsReadSysEnabled;
extern u32 D_8009AFD8;

void CD_init(void);
void CD_initvol(void);
void CdRom_InitCmdState(void);
void CdRom_SetRetryMode(int mode);
void Render_DrawParticleGroup(void);
void CdRom_ReadyEventDispatch(void);
void Render_StepParticlePool(void);
void VSyncCallbacks(int mode, void *callback);

void CdRom_InitDsCallbacks(void) {
    CD_init();
    CD_initvol();

    g_DsReadyCallback = 0;
    g_DsSyncCallback = g_DsReadyCallback;
    /* Match note: g_DsPollCallback clear is in the CdRom_InitCmdState delay slot. */
    asm volatile(
        ".set\tnoreorder\n\t"
        ".set\tnoat\n\t"
        "lui\t$at,%%hi(g_DsPollCallback)\n\t"
        "jal\tCdRom_InitCmdState\n\t"
        "sw\t$zero,%%lo(g_DsPollCallback)($at)\n\t"
        ".set\tat\n\t"
        ".set\treorder"
        :
        :
        : "$31", "memory");
    CdRom_SetRetryMode(0);

    g_CdSyncCallback = (u32)Render_DrawParticleGroup;
    g_CdReadyCallback = (u32)CdRom_ReadyEventDispatch;
    VSyncCallbacks(0, Render_StepParticlePool);

    D_8009AFD8 = 1;
    g_DsReadSysEnabled = 1;
}
