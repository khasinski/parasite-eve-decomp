/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

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
    g_DsPollCallback = 0;
    CdRom_InitCmdState();
    CdRom_SetRetryMode(0);

    g_CdSyncCallback = (CdlCB)Render_DrawParticleGroup;
    g_CdReadyCallback = (CdlCB)CdRom_ReadyEventDispatch;
    VSyncCallbacks(0, LIBDS_DSSYS_1_text_4A4);

    D_8009AFD8 = 1;
    g_DsReadSysEnabled.enabled = 1;
}
