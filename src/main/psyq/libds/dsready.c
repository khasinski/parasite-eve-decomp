/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSREADY.OBJ, part 1 of 5: DsStartReadySystem, DsEndReadySystem, DsReadySystemMode. */
/* DSREADY is split where its functions need different compilers: parts 1
 * and 3 only match with GCC 2.8.1 and -mno-split-addresses, part 4
 * (ER_retry) with GCC 2.8.1, -fno-expensive-optimizations and
 * -fcall-used-$1, parts 2 and 5 with GCC 2.7.2. */
#include "pe1/psyq_ds_queue.h"
#include "pe1/psyq_ds.h"

void ER_cbready(int event, u_char *result);
void LIBDS_DSREADY_text_3D8(u_char event, u_char *result);

int DsStartReadySystem(DsAsyncReadCallback callback, int callbackArg) {
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
    DS_ASYNC_READ_FIELD(state, savedReadyCallback) = DsReadyCallback((DsEventCallback)ER_cbready);
    DS_ASYNC_READ_FIELD(state, savedStartCallback) = DsStartCallback(LIBDS_DSREADY_text_3D8);
    asm volatile("" : "+r"(active));
    DS_ASYNC_READ_FIELD(state, active) = active;
    return 1;
}


void DsEndReadySystem(void) {
    int *state;
    int particleType;
    void *zeroArg1;
    DslCB callback;
    state = &g_DsReadBusy;
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, savedReadyCallback));
        DsStartCallback(DS_ASYNC_READ_FIELD(state, savedStartCallback));
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        callback = 0;
        DsCommand(particleType, zeroArg1, callback, -1);
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}

extern DsCallback D_8009B708;

DsCallback DsReadySystemMode(DsCallback callback) {
    DsCallback *slot;
    DsCallback old;

    slot = &D_8009B708;
    old = *slot;
    *slot = callback;
    return old;
}
