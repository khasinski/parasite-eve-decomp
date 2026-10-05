/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

u32 StGetNext(u32 **addr, u32 **header) {
    u32 **addr_reg;
    u32 **header_reg;
    u8 *entry;
    int index;

    addr_reg = addr;
    index = g_CdStreamRingIndex;
    entry = (u8 *)&StRingAddr[index];

    header_reg = header;
    if (*(u16 *)entry == 1) {
        g_CdStreamRingIndex = 0;
        if (g_CdStreamEndState.endSector != 0) {
            *(u16 *)entry = 0;
        }
        index = g_CdStreamRingIndex;
        entry = (u8 *)&StRingAddr[index];
    }

    asm volatile("" : : : "memory");
    if (*(u16 *)entry == 2) {
        *(u16 *)entry = 4;
        *addr_reg = (u32 *)(&StRingAddr[StRingSize] +
                            g_CdStreamRingIndex * 0x3F);
        *header_reg = (u32 *)entry;
        return 0;
    }
    return 1;
}
