/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/psyq_cd.h"

extern int D_800B8AB0[];
extern u_char D_800A3510[];
extern CdDsReadQueueEntry g_CdDsReadQueue[] __asm__("D_800A3540");
extern int g_CdDsReadIndex __asm__("D_800A3600");
extern int g_CdDsReadQueueState __asm__("D_800A3604");
extern int g_CdPendingReadCount __asm__("D_800A3608");
extern int D_800A3610[];
extern int D_800A3690;

#define W32(ptr, offset) (*(int *)((u_char *)(ptr) + (offset)))

int CdRom_IsDsReadSystemEnabled(void);
void CQ_clear_queue(void *queue);
void CdRom_InitDsCallbacks(void);
void CdRom_SetSyncCallback(unsigned int value);
void CdRom_SetReadyCallback(unsigned int value);
void CdRom_SetDispatchCallback(unsigned int value);
void CdRom_SetPollCallback(unsigned int value);
void Render_StepParticleNode(void);
void Render_PlayParticleEffect(void);
void CdRom_DispatchDsReadyCallback(void);
void CdRom_PollPendingDsRead(void);
void DS_read_cbready(void);
void *DsReadCallback(void *callback);

int CdRom_InitDsReadSystem(void) {
    int i;
    CdDsReadQueueEntry *queue;

    if (CdRom_IsDsReadSystemEnabled() != 0) {
        return 0;
    }

    D_800B8AB0[0] = 0;
    D_800B8AB0[1] = 0;
    D_800B8AB0[2] = 0;

    W32(D_800A3510, 0x20) = 0;
    W32(D_800A3510, 0x10) = 0;
    W32(D_800A3510, 0) = 0;
    D_800A3510[0x24] = 0;
    D_800A3510[0x14] = 0;
    D_800A3510[4] = 0;
    for (i = 0; i < 8; i++) {
        D_800A3510[i + 0x05] = 0;
        D_800A3510[i + 0x15] = 0;
        D_800A3510[i + 0x25] = 0;
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
        D_800A3610[i * 4] = 0;
    }
    D_800A3690 = 0;

    CdRom_InitDsCallbacks();
    CdRom_SetSyncCallback((unsigned int)Render_StepParticleNode);
    CdRom_SetReadyCallback((unsigned int)Render_PlayParticleEffect);
    CdRom_SetDispatchCallback((unsigned int)CdRom_DispatchDsReadyCallback);
    CdRom_SetPollCallback((unsigned int)CdRom_PollPendingDsRead);
    DS_read_cbready();
    DsReadCallback(0);

    return 1;
}
