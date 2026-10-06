/* Psy-Q LIBCD SYS.OBJ: CdStatus, CdMode, CdLastCom. */
/* LIBCD SYS stays in six units: CdLastPos only matches with GCC 2.8.1,
 * sys.c, CdControl and CdMix only with GCC 2.7.2 (CdControl also needs
 * -fno-schedule-insns). */
#include "pe1/psyq_cd.h"

int CdStatus(void) {
    return *(u8 *)&D_8009AFC4;
}

int CdMode(void) {
    return g_CdMode;
}

int CdLastCom(void) {
    return g_CdLastCom;
}
