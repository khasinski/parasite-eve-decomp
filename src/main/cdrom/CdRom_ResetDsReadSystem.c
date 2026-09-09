/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns2 */
#include "pe1/psyq_cd.h"
extern int D_800A3510[];
extern unsigned char D_800A3515[], D_800A3525[], D_800A3535[];
typedef struct { int words[6]; } CdQueue;
typedef struct { int active; int unknown[3]; } CdRequest;
extern CdQueue D_800A3540[];
extern CdRequest D_800A3610[];
extern int D_800A3604, D_800A3600, g_CdPendingReadCount, D_800A3690;
extern void CdRom_AbortCmd(void);
extern void DS_read_cbready(void);
extern void CdRom_EnableDsReadSystem(void);
register int *resetPage asm("$1");

int CdRom_ResetDsReadSystem(void) {
    int i, j, k, offset;
    int *state;
    DsCallbackRegistry *callbacks;
    CdRom_AbortCmd();
    i = 0;
    callbacks = &g_DsReadCallbackState;
    callbacks->start = 0;
    callbacks->sync = 0;
    callbacks->ready = 0;
    state = D_800A3510;
    state[8] = 0;
    state[4] = 0;
    state[0] = 0;
    ((unsigned char *)state)[36] = 0;
    ((unsigned char *)state)[20] = 0;
    ((unsigned char *)state)[4] = 0;
    for (; i < 8; ++i) {
        D_800A3515[i] = 0;
        D_800A3525[i] = 0;
        D_800A3535[i] = 0;
    }
    for (j = 0; j < 8; ++j) CQ_clear_queue(&D_800A3540[j]);
    D_800A3604 = 0;
    D_800A3600 = 0;
    g_CdPendingReadCount = 0;
    for (k = 7, offset = 112; k >= 0; --k, offset -= 16)
        *(int *)((unsigned char *)D_800A3610 + offset) = 0;
    resetPage = (int *)0x800A0000;
    resetPage[0x3690 / 4] = 0;
    DS_read_cbready();
    DsReadCallback(0);
    CdRom_EnableDsReadSystem();
    return 1;
}
