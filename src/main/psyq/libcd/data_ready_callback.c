/* MASPSX_FLAGS: --store-branch-delay */

#include "pe1/psyq_cd.h"

typedef void (*Callback)(void);

extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern unsigned char *g_CdRingBufPtr;
extern CdlLOC D_800A3490;
extern int D_800A3494;
extern Callback g_StrDataReadyCallback;

void data_ready_callback(void) {
    unsigned char *base = g_CdRingBufPtr + (g_CdStreamRingReadSlot << 5);

    *(short *)base = 2;
    __builtin_memcpy(&D_800A3490, base + 0x1C, 4);
    D_800A3494 = *(int *)(base + 8);
    g_CdStreamRingReadSlot = D_800BE998;
    if (g_StrDataReadyCallback != 0) {
        g_StrDataReadyCallback();
    }
    g_CdStreamDataReadyFlag = 0;
}
