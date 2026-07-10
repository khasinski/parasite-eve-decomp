/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned int u32;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Add64(MathU64 *out, u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);

MathU64 *Math_Neg64(MathU64 *out, u32 lo, u32 hi) {
    register MathU64 *out_reg asm("$16");
    volatile MathU64 *tmp;
    volatile u32 one;
    volatile u32 zero;

    out_reg = out;
    one = 1;
    zero = 0;
    tmp = (MathU64 *)&lo;
    tmp->lo = ~lo;
    tmp->hi = ~hi;
    Math_Add64((MathU64 *)tmp, tmp->lo, tmp->hi, one, 0);
    out_reg->lo = tmp->lo;
    out_reg->hi = tmp->hi;
    return out_reg;
}
