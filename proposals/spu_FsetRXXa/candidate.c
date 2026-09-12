/* GAS_VERSION: 2.8.1 */
#include "pe1/psyq_spu_internal.h"

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
