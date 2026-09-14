/* ASSEMBLER: GNU */
#include "pe1/math64.h"
#include "pe1/event.h"

/* Psy-Q LIBMATH MULDF3.OBJ: __muldf3, retained under its historical symbol.
 * The adjacent _mul_mant_d helper still uses a separate GCC 2.8.1 unit. */
double Math_Sqrt64(double x, double y)
{
    struct {
        MathU64 product;
        MathU64 rounding;
        MathU64 a;
        MathU64 b;
        MathU64 temporary;
        MathU64 cross;
    } work;
    MathDoubleBits result;
    unsigned int infinityBits;
    MathDoubleBits left;
    MathDoubleBits right;
    int leftExponent, rightExponent;
    int leftHighIsZero;
    unsigned sign;
    unsigned aHigh;
    unsigned bHigh;
    unsigned aLow;
    unsigned bLow;
    int exponent;
    int scratch;

    left.value = x;
    right.value = y;
    sign = (left.bits.hi & 0x80000000u) ^ (right.bits.hi & 0x80000000u);
    leftHighIsZero = (left.bits.hi & 0x7fffffffu) == 0;
    scratch = leftHighIsZero;
    if ((scratch && left.bits.lo == 0) ||
        ((right.bits.hi & 0x7fffffffu) == 0 && right.bits.lo == 0)) {
        result.bits.hi = sign;
        result.bits.lo = 0;
    } else {
        leftExponent = ((int)left.bits.hi >> 20) & 0x7ff;
        rightExponent = ((int)right.bits.hi >> 20) & 0x7ff;
        exponent = leftExponent + rightExponent - 1022;
        /* Keep the adjusted exponent independent of the shared sum. */
        asm volatile("" : "=r"(exponent) : "0"(exponent));

        work.a.hi = (left.bits.hi & 0xfffff) | 0x100000;
        work.a.lo = left.bits.lo;
        work.b.hi = (right.bits.hi & 0xfffff) | 0x100000;
        work.b.lo = right.bits.lo;
        Math_Shift64(&work.temporary, 1, work.a, 21);
        aHigh = work.temporary.lo;
        Math_Shift64(&work.temporary, 1, work.b, 21);
        bHigh = work.temporary.lo;
        aLow = work.a.lo & 0x1fffff;
        bLow = work.b.lo & 0x1fffff;

        /* Form the retained product from 32-bit high and 21-bit low parts. */
        Math_Mul32To64(&work.product, aHigh, bHigh);
        Math_Mul32To64(&work.cross, aLow, bHigh);
        Math_Shift64(&work.temporary, 1, work.cross, 21);
        scratch = aHigh;
        Math_Add64(&work.product, work.product, work.temporary);
        Math_Mul32To64(&work.cross, bLow, scratch);
        Math_Shift64(&work.temporary, 1, work.cross, 21);
        Math_Add64(&work.product, work.product, work.temporary);
        if ((scratch = work.product.hi & 0x80000000u)) {
            work.rounding.hi = 0;
            work.rounding.lo = 0x400;
            Math_Add64(&work.product, work.product, work.rounding);
            Math_Shift64(&work.product, 1, work.product, 11);
        } else {
            work.rounding.hi = 0;
            work.rounding.lo = 0x200;
            Math_Add64(&work.product, work.product, work.rounding);
            if (work.product.hi & 0x80000000u) {
                Math_Shift64(&work.product, 1, work.product, 11);
            } else {
                Math_Shift64(&work.product, 1, work.product, 10);
                exponent = leftExponent + rightExponent - 1023;
            }
        }

        work.product.hi = work.product.hi & 0xffefffff;
        if (exponent >= 2047) {
            Evt_Deliver(34, 13);
            infinityBits = sign ? 0xfff00000u : 0x7ff00000u;
            result.bits.hi = infinityBits;
            result.bits.lo = 0;
        } else {
            result.bits.hi = (sign | ((unsigned)exponent << 20)) | work.product.hi;
            result.bits.lo = work.product.lo;
        }
    }
    return result.value;
}
