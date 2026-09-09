/* MASPSX_FLAGS: --expand-div */
/* Experimental full TU: six functions match; _spu_FsetRXXa does not. */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_spu_internal.h"

void _spu_FsetRXX(u32 offset, u32 value, u32 mode) {
    if (mode == 0) {
        *(unsigned short *)((unsigned char *)_spu_RXX + offset * 2) = value;
    } else {
        *(unsigned short *)((unsigned char *)_spu_RXX + offset * 2) =
            value >> _spu_mem_mode_plus;
    }
    asm volatile("");
}

u32 _spu_FsetRXXa(s32 arg0, u32 value) {
    s32 offset;
    register u32 shifted asm("$7");
    u32 rem;
    u32 unit;
    register u32 shift asm("$2");
    u32 ret;
    register u32 result asm("$2");

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
        result = ret & 0xFFFF;
        break;
    case -2:
        result = value;
        break;
    default:
    {
        unsigned char *base;
        register u32 addr asm("$3");

        base = _spu_RXX;
        addr = offset * 2;
        addr = addr + (u32) base;
        *(u16 *)addr = shifted;
        result = value;
        break;
    }
    }
    asm volatile("");
    return result;
}

u32 _spu_FgetRXXa(u32 offset, s32 mode)
{
    register u32 value asm("$4") =
        *(volatile u16 *)((unsigned char *)_spu_RXX + offset * 2);
    u32 result;

    if (mode != -1) {
        result = value << _spu_mem_mode_plus;
    } else {
        result = value;
    }
    asm volatile("");
    return result;
}


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


typedef struct SpuWaitWork {
    volatile int i;
    volatile int value;
} SpuWaitWork;

void _spu_Fw1ts(void) {
    SpuWaitWork work;
    int current;

    work.value = 0xD;
    *(int *)&work.i = 0;
    while (work.i < 0x3C) {
        current = work.value;
        work.value = (((current << 1) + current) << 2) + current;
        work.i = work.i + 1;
    }
}
