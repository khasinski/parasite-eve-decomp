typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct MathU64 {
    u32 lo;
    u32 hi;
} MathU64;

MathU64 *Math_Shift64(MathU64 *out, int direction, u32 lo, u32 hi, int amount);
MathU64 *Math_Mul32To64(MathU64 *out, u32 lhs, u32 rhs);
MathU64 *Math_Add64(MathU64 *out, u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi);
int Evt_Deliver(int arg0, int arg1);

#define PACK_DOUBLE(lo_, hi_) (((u64)(hi_) << 32) | (u32)(lo_))

u64 Math_Sqrt64(u32 a_lo, u32 a_hi, u32 b_lo, u32 b_hi) {
    MathU64 tmp;
    MathU64 acc;
    MathU64 product;
    u32 sign;
    u32 mant_a_hi;
    u32 mant_b_hi;
    u32 mant_a_lo;
    u32 mant_b_lo;
    int exponent_sum;
    int exponent;

    sign = (a_hi ^ b_hi) & 0x80000000;
    if (((a_hi & 0x7FFFFFFF) == 0 && a_lo == 0) ||
        ((b_hi & 0x7FFFFFFF) == 0 && b_lo == 0)) {
        return PACK_DOUBLE(0, sign);
    }

    exponent_sum = ((int)a_hi >> 20 & 0x7FF) + ((int)b_hi >> 20 & 0x7FF);
    exponent = exponent_sum - 0x3FE;

    mant_a_lo = a_lo;
    mant_b_lo = b_lo;
    mant_a_hi = (a_hi & 0xFFFFF) | 0x100000;
    mant_b_hi = (b_hi & 0xFFFFF) | 0x100000;

    Math_Shift64(&tmp, 1, mant_a_lo, mant_a_hi, 0x15);
    mant_a_hi = tmp.lo;
    Math_Shift64(&tmp, 1, mant_b_lo, mant_b_hi, 0x15);

    mant_a_lo &= 0x1FFFFF;
    mant_b_lo &= 0x1FFFFF;
    mant_b_hi = tmp.lo;

    Math_Mul32To64(&acc, mant_a_hi, mant_b_hi);
    Math_Mul32To64(&product, mant_a_lo, mant_b_hi);
    Math_Shift64(&tmp, 1, product.lo, product.hi, 0x15);
    Math_Add64(&acc, acc.lo, acc.hi, tmp.lo, tmp.hi);

    Math_Mul32To64(&product, mant_b_lo, mant_a_hi);
    Math_Shift64(&tmp, 1, product.lo, product.hi, 0x15);
    Math_Add64(&acc, acc.lo, acc.hi, tmp.lo, tmp.hi);

    if ((int)acc.hi < 0) {
        Math_Add64(&acc, acc.lo, acc.hi, 0x400, 0);
        Math_Shift64(&acc, 1, acc.lo, acc.hi, 0xB);
    } else {
        Math_Add64(&acc, acc.lo, acc.hi, 0x200, 0);
        if (acc.hi & 0x80000000) {
            Math_Shift64(&acc, 1, acc.lo, acc.hi, 0xB);
        } else {
            exponent = exponent_sum - 0x3FF;
            Math_Shift64(&acc, 1, acc.lo, acc.hi, 0xA);
        }
    }

    acc.hi &= 0xFFEFFFFF;
    if (exponent >= 0x7FF) {
        Evt_Deliver(0x22, 0xD);
        if (sign != 0) {
            return PACK_DOUBLE(0, 0xFFF00000);
        }
        return PACK_DOUBLE(0, 0x7FF00000);
    }

    return PACK_DOUBLE(acc.lo, sign | ((u32)exponent << 20) | acc.hi);
}
