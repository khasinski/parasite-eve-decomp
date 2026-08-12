#include "pe1/math64.h"

double Math_Int32ToDouble(int value) {
    MathDoubleBits result;
    MathU64 mantissa;
    unsigned int sign;
    int exponent;

    sign = 0;
    exponent = 0x41d;
    if (value == 0) {
        result.bits.lo = 0;
        result.bits.hi = 0;
        return result.value;
    }

    if (value < 0) {
        sign = 0x80000000;
        value = -value;
    }
    while (value <= 0xffffff) {
        value <<= 4;
        exponent -= 4;
    }
    while (value <= 0x3fffffff) {
        value <<= 1;
        exponent--;
    }

    mantissa.lo = 0;
    mantissa.hi = value;
    mantissa = Math_Shift64(1, mantissa, 10);
    mantissa.hi &= 0xffefffff;
    result.bits.hi = mantissa.hi | sign;
    result.bits.hi |= exponent << 20;
    result.bits.lo = mantissa.lo;
    return result.value;
}
