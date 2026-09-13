/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern unsigned char D_800A3515[], D_800A3525[], D_800A3535[];
extern DsReadCallbackSlot D_800A3610[];
extern int D_800A3604, D_800A3600, g_CdPendingReadCount, D_800A3690;
extern void DS_read_cbready(void);

/* Initialize the DS queues and callbacks only when the subsystem is disabled.
 * Reserved bytes in command and callback records retain their old contents. */
int CdRom_InitDsReadSystem(void) {
    int i, j, k, offset;
    CdQueuedCmdSlot *state;
    DsCallbackRegistry *callbacks;
    if (CdRom_IsDsReadSystemEnabled()) return 0;
    i = 0;
    callbacks = &g_DsReadCallbackState;
    callbacks->start = 0;
    callbacks->sync = 0;
    callbacks->ready = 0;
    state = g_CdQueuedCmdSlots;
    state[2].state = 0;
    state[1].state = 0;
    state[0].state = 0;
    state[2].result = 0;
    state[1].result = 0;
    state[0].result = 0;
    for (; i < 8; ++i) {
        D_800A3515[i] = 0;
        D_800A3525[i] = 0;
        D_800A3535[i] = 0;
    }
    for (j = 0; j < 8; ++j) CQ_clear_queue(&g_CdDsReadQueue[j]);
    D_800A3604 = 0;
    D_800A3600 = 0;
    g_CdPendingReadCount = 0;
    for (k = 7, offset = 112; k >= 0; --k, offset -= 16)
        ((DsReadCallbackSlot *)((unsigned char *)D_800A3610 + offset))->value = 0;
    D_800A3690 = 0;
    CdRom_InitDsCallbacks();
    CdRom_SetSyncCallback((unsigned int)LIBDS_DSSYS_2_text_3D0);
    CdRom_SetReadyCallback((unsigned int)Render_PlayParticleEffect);
    CdRom_SetDispatchCallback((unsigned int)CdRom_DispatchDsReadyCallback);
    CdRom_SetPollCallback((unsigned int)CdRom_PollPendingDsRead);
    DS_read_cbready();
    DsReadCallback(0);
    return 1;
}
