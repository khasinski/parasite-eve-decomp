/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-expensive-optimizations */

#include "pe1/psyq_cd.h"

int CD_vol(CdlATV *vol) {
    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = vol->val0;
    *g_CdRegResponse = vol->val1;
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = vol->val2;
    *g_CdRegDataWrite = vol->val3;
    *g_CdRegResponse = 0x20;
    return 0;
}

void CD_flush(void) {
    register CdInterruptEvents *state asm("$3");

    *g_CdRegIndexBase = 1;
    while (*g_CdRegResponse & 7) {
        *g_CdRegIndexBase = 1;
        *g_CdRegResponse = 7;
        *g_CdRegDataWrite = 7;
    }
    state = &D_8009B294;
    asm volatile("" : "=r"(state) : "0"(state));
    state->end = 0;
    state->ready = state->end;
    {
        volatile u8 *index = g_CdRegIndexBase;
        state->sync = 2;
        *index = 0;
    }
    *g_CdRegResponse = 0;
    *g_CdRegRequest = 0x1325;
}
