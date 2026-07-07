
#include "pe1/psyq_spu_internal.h"

u32 _spu_FsetRXXa(s32 arg0, u32 value) {
    register s32 offset asm("$6");
    register u32 shifted asm("$7");
    u32 rem;
    register u32 unit asm("$4");
    u32 shift;
    u32 ret;

    offset = arg0;

    if (_spu_mem_mode != 0) {
        unit = _spu_mem_mode_unit;
        asm volatile(
            "divu\t$0,%1,%2\n\t"
            "bnez\t%2,1f\n\t"
            "nop\n\t"
            ".word\t0x0007000d\n"
            "1:\n\t"
            "mfhi\t%0"
            : "=r"(rem)
            : "r"(value), "r"(unit));
        if (rem != 0) {
            value += unit;
            value &= ~_spu_mem_mode_unitM;
        }
    }

    shift = _spu_mem_mode_plus;
    asm volatile(
        "nop\n\t"
        "srlv\t%0,%1,%2"
        : "=r"(shifted)
        : "r"(value), "r"(shift));
    ret = shifted;

    switch (offset) {
    case -1:
        return ret & 0xFFFF;
    case -2:
        return value;
    default:
    {
        unsigned char *base;
        register u32 addr asm("$3");

        base = _spu_RXX;
        addr = offset * 2;
        addr = addr + (u32) base;
        *(u16 *)addr = shifted;
        return value;
    }
    }
}
