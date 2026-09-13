/* ASSEMBLER: GNU */
/* Complete Psy-Q LIBCD C_004.OBJ code; trailing object alignment is in the manifest. */
#include "pe1/psyq_cd.h"

void data_ready_callback(void) {
    StHEADER *base =
        (StHEADER *)(g_CdRingBufPtr + (g_CdStreamRingReadSlot * sizeof(StHEADER)));
    int sector_data;
    int next_slot;
    DsCallback callback;

    base->id = 2;
    __builtin_memcpy(&D_800A3490, &base->loc, 4);
    sector_data = base->frameCount;
    /* Retain the frame load before fetching the next ring slot. */
    asm volatile("" : "=r"(sector_data) : "0"(sector_data));
    next_slot = D_800BE998;
    callback = g_StrDataReadyCallback;
    D_800A3494 = sector_data;
    g_CdStreamRingReadSlot = next_slot;
    if (callback != 0) {
        callback();
    }
    g_CdStreamDataReadyFlag = 0;
}

int StGetBackloc(CdlLOC *arg0) {
    CdlLOC *saved_arg = arg0;

    if (D_800A8020 != 0) {
        return -1;
    }

    CdIntToPos_Local(CdPosToInt_Local(&D_800A3490) + 1, saved_arg);
    return D_800A3494;
}
