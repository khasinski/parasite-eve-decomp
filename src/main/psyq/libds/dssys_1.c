/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 1 of 11: DS_init. */
/* DSSYS_1 is split where its functions need different options: parts 2, 5,
 * 7, 9 and 11 only match with GCC 2.8.1 and -mno-split-addresses (2.7.2
 * cannot keep the shared address base in a register), part 3 needs
 * -fno-schedule-insns2, the rest match with GCC 2.7.2. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"

int CD_init(void);
int CD_initvol(void);
void DS_reset_members(void);
void DsReadMode(int mode);
void LIBDS_DSSYS_1_text_7FC(int, void *);
void LIBDS_DSSYS_1_text_E10(int, u_char *);
void LIBDS_DSSYS_1_text_4A4(void);
PsyqInterruptHandler VSyncCallbacks(unsigned int channel, PsyqInterruptHandler callback);

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
