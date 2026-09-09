/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_cd.h"

void CdRom_AsyncCallback(u_char event, u_char *result);
void CdRom_ReadDoneCallback(u_char event, u_char *result);

int CdRom_InitAsyncRead(DsAsyncReadCallback callback, int callbackArg) {
    int *state;
    int active;

    state = &g_DsReadBusy;
    active = 1;
    if (DS_ASYNC_READ_FIELD(state, active) == active) {
        return 0;
    }

    DS_ASYNC_READ_FIELD(state, result) = -1;
    DS_ASYNC_READ_FIELD(state, reserved04) = 0;
    DS_ASYNC_READ_FIELD(state, reserved0C) = 0;
    DS_ASYNC_READ_FIELD(state, callback) = callback;
    DS_ASYNC_READ_FIELD(state, callback_arg) = callbackArg;
    DS_ASYNC_READ_FIELD(state, saved_sync_callback) = DsSyncCallback(CdRom_AsyncCallback);
    DS_ASYNC_READ_FIELD(state, saved_ready_callback) = DsReadyCallback(CdRom_ReadDoneCallback);
    asm volatile("" : "+r"(active));
    DS_ASYNC_READ_FIELD(state, active) = active;
    return 1;
}


void DsReadBreak(void) {
    int *state;
    int particleType;
    void *zeroArg1;
    int zeroArg2;
    state = &g_DsReadBusy;
    if (DS_ASYNC_READ_FIELD(state, active) == 1) {
        DsSyncCallback(DS_ASYNC_READ_FIELD(state, saved_sync_callback));
        DsReadyCallback(DS_ASYNC_READ_FIELD(state, saved_ready_callback));
        particleType = 9;
        zeroArg1 = 0;
        asm volatile("" : "+r"(particleType), "+r"(zeroArg1));
        zeroArg2 = 0;
        Render_AllocParticleNode(particleType, zeroArg1, zeroArg2, -1);
    }
    DS_ASYNC_READ_FIELD(state, active) = 0;
}
