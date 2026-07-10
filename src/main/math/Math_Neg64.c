/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned int u32;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Add64(MathU64 *out, u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);

MathU64 *Math_Neg64(MathU64 *out, u32 lo, u32 hi) {
    register MathU64 *out_reg asm("$16");
    register u32 add_lo asm("$5");
    register u32 add_hi asm("$6");
    register u32 add_one asm("$7");
    volatile MathU64 *tmp;
    register u32 result_lo asm("$2");
    register u32 result_hi asm("$3");
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
