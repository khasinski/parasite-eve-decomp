/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-expensive-optimizations */
#include "pe1/psyq_cd.h"

void Cd_SetIntrMask(void);

void CD_initintr(void) {
    g_CdReadyCallback = 0;
    g_CdSyncCallback = 0;
    D_8009AFC8 = 0;
    D_8009AFC4 = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);
}
