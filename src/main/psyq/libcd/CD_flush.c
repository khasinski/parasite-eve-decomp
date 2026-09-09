#include "pe1/psyq_cd.h"

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
