#include "pe1/psyq_cd.h"
extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegDataWrite;
extern volatile u8 *g_CdRegResponse;
extern s32 *g_CdRegRequest;

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
