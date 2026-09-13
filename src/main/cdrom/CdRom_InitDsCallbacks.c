#include "pe1/psyq_cd.h"

extern volatile u32 g_DsPollCallback;
extern u32 D_8009AFD8;

void CD_init(void);
void CD_initvol(void);
void CdRom_InitCmdState(void);
void CdRom_SetRetryMode(int mode);
void Render_DrawParticleGroup(int, void *);
void CdRom_ReadyEventDispatch(int, u_char *);
void LIBDS_DSSYS_1_text_4A4(void);
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

    g_CdSyncCallback = (CdlCB)Render_DrawParticleGroup;
    g_CdReadyCallback = (CdlCB)CdRom_ReadyEventDispatch;
    VSyncCallbacks(0, LIBDS_DSSYS_1_text_4A4);

    D_8009AFD8 = 1;
    g_DsReadSysEnabled.enabled = 1;
}
