/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_ds_queue.h"

void LIBDS_DSREADY_text_FC(int event, u_char *result);
void CdRom_ReadDoneCallback(u_char event, u_char *result);

int CdRom_InitAsyncRead(DsAsyncReadCallback callback, int callbackArg) {
    int *state;
    int active;

    state = &g_DsReadBusy;
    active = 1;
    if (DS_ASYNC_READ_FIELD(state, active) == active) {
        return 0;
    }

    DS_ASYNC_READ_FIELD(state, nextSector) = -1;
    DS_ASYNC_READ_FIELD(state, lastDeliveredSector) = 0;
    DS_ASYNC_READ_FIELD(state, retryPending) = 0;
    DS_ASYNC_READ_FIELD(state, callback) = callback;
    DS_ASYNC_READ_FIELD(state, retriesRemaining) = callbackArg;
    DS_ASYNC_READ_FIELD(state, savedSyncCallback) = DsSyncCallback((DsEventCallback)LIBDS_DSREADY_text_FC);
    DS_ASYNC_READ_FIELD(state, savedReadyCallback) = DsReadyCallback(CdRom_ReadDoneCallback);
    asm volatile("" : "+r"(active));
    DS_ASYNC_READ_FIELD(state, active) = active;
    return 1;
}


void DsReadBreak(void) {
    int *state;
    int particleType;
    void *zeroArg1;
    DslCB callback;
    state = &g_DsReadBusy;
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsSyncCallback(DS_ASYNC_READ_FIELD(state, savedSyncCallback));
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, savedReadyCallback));
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        callback = 0;
        Render_AllocParticleNode(particleType, zeroArg1, callback, -1);
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}
