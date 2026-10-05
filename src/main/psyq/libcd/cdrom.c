/* ASSEMBLER: GNU */

#include "pe1/psyq_cd.h"

void StSetRing(u32 *ring_addr, u32 ring_size) {
    D_800C0DC8 = (StHEADER *)ring_addr;
    D_800C20C4 = ring_size;
    StClearRing();
}
