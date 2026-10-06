/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSREADY.OBJ, part 5 of 5: ER_cbsync, ER_active, ER_clear. */
#include "pe1/psyq_ds.h"

void ER_cbready(int event, u_char *result);

void ER_cbsync(u_char event) {
    if (event == 2) {
        DsReadyCallback((DsEventCallback)ER_cbready);
    }
}

int ER_active(void) {
    return g_DsReadBusy;
}

void ER_clear(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, savedReadyCallback));
        DsStartCallback(DS_ASYNC_READ_FIELD(state, savedStartCallback));
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}
