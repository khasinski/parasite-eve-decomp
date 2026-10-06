/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns2 */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

/* These adjacent retail functions share DS queue reset state:
 * DsInit at 0x8007EC14 and DsReset at
 * 0x8007ED58. Their combined GNU 2.8.1 object preserves both code ranges. */
extern unsigned char D_800A3515[], D_800A3525[], D_800A3535[];
extern DsReadCallbackSlot D_800A3610[];
extern int D_800A3604, D_800A3600, g_CdPendingReadCount, D_800A3690;
extern void DS_stop(void);
int DsInit(void) {
    int i, j, k, offset;
    CdQueuedCmdSlot *state;
    DsCallbackRegistry *callbacks;
    if (DS_system_active()) return 0;
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
    DS_init();
    DS_sync_callback((unsigned int)CQ_sync_system);
    DS_ready_callback((unsigned int)CQ_ready_system);
    DS_start_callback((unsigned int)LIBDS_DSSYS_2_text_13CC);
    DS_vsync_callback((unsigned int)CQ_vsync_system);
    DS_read_cbready();
    DsReadCallback(0);
    return 1;
}

int DsReset(void) {
    int i, j, k, offset;
    CdQueuedCmdSlot *state;
    DsCallbackRegistry *callbacks;
    DS_stop();
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
        *(int *)((unsigned char *)D_800A3610 + offset) = 0;
    D_800A3690 = 0;
    DS_read_cbready();
    DsReadCallback(0);
    DS_restart();
    return 1;
}
