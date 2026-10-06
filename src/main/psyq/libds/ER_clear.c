/* GCC_VERSION: 2.8.1 */
/* Psy-Q LIBDS DSREADY.OBJ: ER_clear. */

#include "pe1/psyq_ds.h"


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
