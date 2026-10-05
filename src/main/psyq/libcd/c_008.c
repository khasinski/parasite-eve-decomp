#include "pe1/psyq_cd.h"

void init_ring_status(int start, u32 count) {
    u32 i;

    i = 0;
    if (count != 0) {
        do {
            *(s32 *)((u8 *)StRingAddr + ((i + start) << 5)) = 0;
            i++;
        } while (i < count);
    }
}
