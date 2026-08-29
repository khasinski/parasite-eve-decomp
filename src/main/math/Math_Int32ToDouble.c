#include "common.h"
#include "pe1/math64.h"

/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

double Math_Int32ToDouble(int value)
{
    MathDoubleBits result;
    MathU64 parts;
    u32 sign;
    u32 exponent;
    s32 magnitude;

    sign = 0;
    exponent = 0x41D;
    if (value == 0) {
        result.bits.lo = 0;
        result.bits.hi = 0;
    } else {
        if (value < 0) {
            sign = 0x80000000;
            value = -value;
        }

        magnitude = value;
        if (magnitude <= 0x00FFFFFF) {
            do {
                magnitude <<= 4;
                exponent -= 4;
            } while (magnitude <= 0x00FFFFFF);
        }

        if (magnitude <= 0x3FFFFFFF) {
            do {
                magnitude <<= 1;
                exponent--;
            } while (magnitude <= 0x3FFFFFFF);
        }

        parts.lo = 0;
        parts.hi = magnitude;
        parts = Math_Shift64(1, parts, 10);
        parts.hi &= 0xFFEFFFFF;
        result.bits.hi = parts.hi | sign;
        result.bits.hi |= exponent << 20;
        result.bits.lo = parts.lo;
    }

    return result.value;
}
