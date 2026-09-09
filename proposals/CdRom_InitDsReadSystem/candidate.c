/* GCC_VERSION: 2.8.1 */
#include "pe1/psyq_cd.h"

int CdRom_InitDsReadSystem(void) {
    int i;
    CdQueuedCmdSlot *slots;
    DsCallbackRegistry *callbacks;
    if (CdRom_IsDsReadSystemEnabled()) return 0;
    callbacks = &g_DsReadCallbackState;
    callbacks->start = 0;
    callbacks->sync = 0;
    callbacks->ready = 0;
    slots = g_CdQueuedCmdSlots;
    slots[2].state = 0;
    slots[1].state = 0;
    slots[0].state = 0;
    slots[2].result = 0;
    slots[1].result = 0;
    slots[0].result = 0;
    for (i = 0; i < 8; i++) {
        slots[0].payload[i] = 0;
        slots[1].payload[i] = 0;
        slots[2].payload[i] = 0;
    }
    for (i = 0; i < 8; i++) CQ_clear_queue(&g_CdDsReadQueue[i]);
    g_CdDsReadIndex = 0;
    g_CdDsReadQueueState = 0;
    g_CdPendingReadCount = 0;
    for (i = 7; i >= 0; i--) g_DsReadCallbackSlots[i].value = 0;
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
