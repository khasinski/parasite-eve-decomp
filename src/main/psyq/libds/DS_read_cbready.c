/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"

void DsSyncCallback(int arg0);
void DsReadyCallback(int arg0);

void DS_read_cbready(void) {
    int *state;

    state = &g_DsReadBusy;
    asm volatile("" : "=r"(state) : "0"(state));
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsSyncCallback(DS_ASYNC_READ_FIELD(state, saved_sync_callback));
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, saved_ready_callback));
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}
