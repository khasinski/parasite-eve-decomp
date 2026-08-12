/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

int CdRom_InitDsReadSystem(void)
{
    CdDsReadQueueEntry *queue;
    int i;

    if (CdRom_IsDsReadSystemEnabled() != 0) {
        return 0;
    }

    g_DsReadCallbackState[0] = 0;
    g_DsReadCallbackState[1] = 0;
    g_DsReadCallbackState[2] = 0;

    g_CdQueuedCmdSlots[2].state = 0;
    g_CdQueuedCmdSlots[1].state = 0;
    g_CdQueuedCmdSlots[0].state = 0;
    g_CdQueuedCmdSlots[2].result = 0;
    g_CdQueuedCmdSlots[1].result = 0;
    g_CdQueuedCmdSlots[0].result = 0;
    for (i = 0; i < 8; i++) {
        g_CdQueuedCmdSlots[0].payload[i] = 0;
        g_CdQueuedCmdSlots[1].payload[i] = 0;
        g_CdQueuedCmdSlots[2].payload[i] = 0;
    }

    queue = g_CdDsReadQueue;
    for (i = 0; i < 8; i++) {
        CQ_clear_queue(queue);
        queue++;
    }

    g_CdDsReadQueueState = 0;
    g_CdDsReadIndex = 0;
    g_CdPendingReadCount = 0;
    for (i = 7; i >= 0; i--) {
        g_DsReadCallbackSlots[i].value = 0;
    }
    g_DsReadCallbackCursor = 0;

    CdRom_InitDsCallbacks();
    CdRom_SetSyncCallback((unsigned int)Render_StepParticleNode);
    CdRom_SetReadyCallback((unsigned int)Render_PlayParticleEffect);
    CdRom_SetDispatchCallback((unsigned int)CdRom_DispatchDsReadyCallback);
    CdRom_SetPollCallback((unsigned int)CdRom_PollPendingDsRead);
    DS_read_cbready();
    DsReadCallback(0);

    return 1;
}
