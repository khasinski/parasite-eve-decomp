#include "pe1/psyq_spu_internal.h"

void _spu_FsetRXX(u32 offset, u32 value, u32 mode) {
    if (mode == 0) {
        *(unsigned short *)(_spu_RXX + offset * 2) = value;
    } else {
        *(unsigned short *)(_spu_RXX + offset * 2) = value >> _spu_mem_mode_plus;
    }
}
