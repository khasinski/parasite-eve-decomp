#include "pe1/psyq_cd.h"

void StClearRing(void) {
    u32 size = StRingSize;

    g_CdStreamRingIndex = 0;
    g_CdStreamRingReadSlot = 0;
    D_800BE998 = 0;
    g_CdStreamDataReadyFlag = 0;
    init_ring_status(0, size);
    D_800B0CD0.word = 0;
    D_800A8018 = 0;
    D_800A5D54 = 0;
}
