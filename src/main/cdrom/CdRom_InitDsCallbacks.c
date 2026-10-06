/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

void CD_init(void);
void CD_initvol(void);
void DS_reset_members(void);
void DsReadMode(int mode);
void LIBDS_DSSYS_1_text_7FC(int, void *);
void LIBDS_DSSYS_1_text_E10(int, u_char *);
void LIBDS_DSSYS_1_text_4A4(void);
void VSyncCallbacks(int mode, void *callback);

void DS_init(void) {
    CD_init();
    CD_initvol();

    g_DsReadyCallback = 0;
    g_DsSyncCallback = g_DsReadyCallback;
    g_DsPollCallback = 0;
    DS_reset_members();
    DsReadMode(0);

    g_CdSyncCallback = (CdlCB)LIBDS_DSSYS_1_text_7FC;
    g_CdReadyCallback = (CdlCB)LIBDS_DSSYS_1_text_E10;
    VSyncCallbacks(0, LIBDS_DSSYS_1_text_4A4);

    D_8009AFD8 = 1;
    g_DsReadSysEnabled.enabled = 1;
}
