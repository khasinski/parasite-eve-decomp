#include "pe1/psyq_cd.h"

extern int g_CdStreamRingWritePtr;
extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern int D_800B0CD0;
extern short g_StStreamState;
extern int g_StStreamReadState;

void StClearRing(void) {
    u32 size = StRingSize;

    g_CdStreamRingWritePtr = 0;
    g_CdStreamRingReadSlot = 0;
    D_800BE998 = 0;
    g_CdStreamDataReadyFlag = 0;
    init_ring_status(0, size);
    D_800B0CD0 = 0;
    g_StStreamState = 0;
    g_StStreamReadState = 0;
}
