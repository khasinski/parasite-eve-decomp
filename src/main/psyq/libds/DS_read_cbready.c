/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_ds.h"


void DS_read_cbready(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsSyncCallback(DS_ASYNC_READ_FIELD(state, savedSyncCallback));
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, savedReadyCallback));
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}
