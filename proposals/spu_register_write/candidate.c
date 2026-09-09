/* MASPSX_FLAGS: --expand-div */
#include "pe1/psyq_spu_internal.h"

void _spu_FsetRXX(u32 offset, u32 value, u32 mode) {
    if (mode == 0) {
        ((u16 *)_spu_RXX)[offset] = value;
    } else {
        ((u16 *)_spu_RXX)[offset] =
            value >> _spu_mem_mode_plus;
    }
}

u32 _spu_FsetRXXa(s32 arg0, u32 value) {
    register s32 offset asm("$6");
    register u32 shifted asm("$7");
    u32 rem;
    register u32 unit asm("$4");
    register u32 shift asm("$2");
    u32 ret;

    offset = arg0;

    if (_spu_mem_mode != 0) {
        unit = _spu_mem_mode_unit;
        rem = value % unit;
        if (rem != 0) {
            value += unit;
            value &= ~_spu_mem_mode_unitM;
        }
    }

    shift = _spu_mem_mode_plus;
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
        SpuRegs *base;
        register u32 addr asm("$3");

        base = _spu_RXX;
        addr = offset * 2;
        addr = addr + (u32) base;
        *(u16 *)addr = shifted;
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
    return (u32)value << _spu_mem_mode_plus;
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

#include "../_spu_Fw1ts/candidate.c"
