#include "pe1/psyq_spu_internal.h"

u32 _spu_FgetRXXa(u32 offset, s32 mode) {
    register u32 value asm("$4");

    value = *(u16 *)(_spu_RXX + offset * 2);

    if (mode == -1) {
        return value;
    }
    return value << _spu_mem_mode_plus;
}
