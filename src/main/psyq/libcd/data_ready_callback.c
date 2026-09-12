/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

typedef void (*Callback)(void);

extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern unsigned char *g_CdRingBufPtr;
extern CdlLOC D_800A3490;
extern Callback g_StrDataReadyCallback;

extern int D_800A3494;

void data_ready_callback(void) {
    unsigned char *base = g_CdRingBufPtr + (g_CdStreamRingReadSlot << 5);
    int sector_data;
    int next_slot;
    Callback callback;

    *(short *)base = 2;
    __builtin_memcpy(&D_800A3490, base + 0x1C, 4);
    sector_data = *(int *)(base + 8);
    next_slot = D_800BE998;
    callback = g_StrDataReadyCallback;
    D_800A3494 = sector_data;
    g_CdStreamRingReadSlot = next_slot;
    if (callback != 0) {
        callback();
    }
    g_CdStreamDataReadyFlag = 0;
}
