/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned int u32;

#include "pe1/math64.h"

MathU64 *Math_Add64(MathU64 *out, u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);

MathU64 *Math_Neg64(MathU64 *out, u32 lo, u32 hi) {
    MathU64 *out_reg;
    u32 add_lo;
    u32 add_hi;
    u32 add_one;
    volatile MathU64 *tmp;
    u32 result_lo;
    u32 result_hi;
    volatile u32 one;
    volatile u32 zero;

    out_reg = out;
    tmp = (MathU64 *)&lo;
    tmp->hi = hi;
    zero = 0;
    one = 1;
    tmp->hi = ~hi;
    tmp->lo = ~lo;
    add_one = one;
    add_lo = tmp->lo;
    add_hi = tmp->hi;
    Math_Add64((MathU64 *)tmp, add_lo, add_hi, add_one, 0);
    result_lo = tmp->lo;
    result_hi = tmp->hi;
    out_reg->lo = result_lo;
    out_reg->hi = result_hi;
    asm volatile("" : : "r"(out_reg) : "memory");
    return out_reg;
}
