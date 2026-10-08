/* PSY-Q LIBSPU SPU, part 4 of 5: _spu_FsetRXX, _spu_FsetRXXa, _spu_FgetRXXa,
 * _spu_FsetPCR, _spu_FsetDelayW, _spu_FsetDelayR. */
/* GAS_VERSION: 2.8.1 */
#include "pe1/psyq_spu_internal.h"

void _spu_FsetRXX(u32 offset, u32 value, u32 mode) {
    if (mode == 0) {
        ((u16 *)_spu_RXX)[offset] = value;
    } else {
        ((u16 *)_spu_RXX)[offset] =
            value >> _spu_mem_mode.shift;
    }
}

u32 _spu_FsetRXXa(s32 arg0, u32 value) {
    s32 offset;
    register u32 shifted asm("$7");
    u32 rem;
    u32 unit;
    register u32 shift asm("$2");
    u32 ret;

    offset = arg0;

    if (_spu_mem_mode.mode != 0) {
        unit = _spu_mem_mode.bytes;
        rem = value % unit;
        if (rem != 0) {
            value += unit;
            value &= ~_spu_mem_mode.mask;
        }
    }

    shift = _spu_mem_mode.shift;
    shifted = value >> shift;
    asm volatile("" : "=r"(shifted) : "0"(shifted));
    ret = shifted;

    switch (offset) {
    case -1:
        return ret & 0xFFFF;
    case -2:
        return value;
    default:
    {
        ((u16 *)_spu_RXX)[offset] = shifted;
        return value;
    }
    }
}

u32 _spu_FgetRXXa(u32 offset, s32 mode)
{
    u16 value = ((volatile u16 *)_spu_RXX)[offset];
    if (mode == -1) {
        return value;
    }
    return value << _spu_mem_mode.shift;
}

#include "pe1/psyq_spu_internal.h"

void _spu_FsetPCR(s32 high_priority) {
    *_spu_sys_pcr &= 0xFFF8FFFF;

    if (high_priority) {
        *_spu_sys_pcr |= 0x30000;
    } else {
        *_spu_sys_pcr |= 0x50000;
    }
}

void _spu_FsetDelayW(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x20000000;
}

void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x22000000;
}
