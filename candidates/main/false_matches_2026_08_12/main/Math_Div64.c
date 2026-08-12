#include "pe1/event.h"
#include "pe1/math64.h"

double Math_Div64(double numerator, double denominator) {
    MathDoubleBits dividend;
    MathDoubleBits divisor;
    MathDoubleBits output;
    MathU64 dividendMantissa;
    MathU64 divisorMantissa;
    MathU64 quotient;
    MathU64 bit;
    MathU64 negativeDivisor;
    MathU64 rounding;
    unsigned int sign;
    int exponent;
    int shift;

    dividend.value = numerator;
    divisor.value = denominator;
    exponent = (((int)dividend.bits.hi >> 20) & 0x7ff) -
               (((int)divisor.bits.hi >> 20) & 0x7ff) + 0x3fe;
    sign = (dividend.bits.hi & 0x80000000) ^
           (divisor.bits.hi & 0x80000000);

    if ((divisor.bits.hi & 0x7fffffff) == 0 && divisor.bits.lo == 0) {
        output.bits.lo = 0xffffffff;
        output.bits.hi = sign != 0 ? 0xffffffff : 0x7fffffff;
        return output.value;
    }
    if ((dividend.bits.hi & 0x7fffffff) == 0 && dividend.bits.lo == 0) {
        output.bits.lo = 0;
        output.bits.hi = sign;
        return output.value;
    }

    dividendMantissa.lo = dividend.bits.lo;
    dividendMantissa.hi = (dividend.bits.hi & 0xfffff) | 0x100000;
    divisorMantissa.lo = divisor.bits.lo;
    divisorMantissa.hi = (divisor.bits.hi & 0xfffff) | 0x100000;
    if (Math_Cmp64Pair(dividendMantissa, divisorMantissa) < 0) {
        dividendMantissa = Math_Add64WithShift(0, dividendMantissa, 1);
        exponent--;
    }

    quotient.lo = 0;
    quotient.hi = 0;
    bit.lo = 0;
    bit.hi = 0x200000;
    do {
        if (Math_Cmp64Pair(dividendMantissa, divisorMantissa) >= 0) {
            quotient.lo |= bit.lo;
            quotient.hi |= bit.hi;
            negativeDivisor = Math_Neg64(divisorMantissa);
            dividendMantissa = Math_Add64(dividendMantissa, negativeDivisor);
        }
        dividendMantissa = Math_Add64WithShift(0, dividendMantissa, 1);
        bit = Math_Shift64(1, bit, 1);
    } while (bit.hi != 0 || bit.lo != 0);

    rounding.hi = 0;
    if (exponent >= 0) {
        rounding.lo = 1;
        quotient = Math_Add64(quotient, rounding);
        exponent++;
        shift = 1;
    } else {
        shift = -exponent;
        rounding.lo = 1 << shift;
        quotient = Math_Add64(quotient, rounding);
        exponent = 0;
        shift++;
    }
    quotient = Math_Shift64(1, quotient, shift);
    quotient.hi &= 0xffefffff;

    if (exponent >= 0x7ff) {
        Evt_Deliver(0x22, 0xf);
        output.bits.lo = 0;
        output.bits.hi = sign == 0 ? 0x7ff00000 : 0xfff00000;
        return output.value;
    }

    output.bits.lo = quotient.lo;
    output.bits.hi = sign | (exponent << 20) | quotient.hi;
    return output.value;
}
