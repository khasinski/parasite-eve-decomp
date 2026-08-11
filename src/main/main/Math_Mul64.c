#include "pe1/event.h"
#include "pe1/math64.h"

double Math_Mul64(double left, double right) {
    MathDoubleBits lhs;
    MathDoubleBits rhs;
    MathDoubleBits output;
    MathU64 lhsMantissa;
    MathU64 rhsMantissa;
    MathU64 rounding;
    unsigned int sign;
    int lhsExponent;
    int rhsExponent;

    lhs.value = left;
    rhs.value = right;
    output.bits.lo = 0;
    output.bits.hi = 0;
    sign = 0;

    if ((lhs.bits.hi & 0x7fffffff) == 0 && lhs.bits.lo == 0) {
        return rhs.value;
    }
    if ((rhs.bits.hi & 0x7fffffff) == 0 && rhs.bits.lo == 0) {
        return lhs.value;
    }

    lhsExponent = ((int)lhs.bits.hi >> 20) & 0x7ff;
    rhsExponent = ((int)rhs.bits.hi >> 20) & 0x7ff;
    if (rhsExponent + 54 < lhsExponent) {
        return lhs.value;
    }
    if (lhsExponent + 54 < rhsExponent) {
        return rhs.value;
    }

    lhsMantissa.lo = lhs.bits.lo;
    lhsMantissa.hi = (lhs.bits.hi & 0xfffff) | 0x100000;
    rhsMantissa.lo = rhs.bits.lo;
    rhsMantissa.hi = (rhs.bits.hi & 0xfffff) | 0x100000;
    if ((int)lhs.bits.hi < 0) {
        lhsMantissa = Math_Neg64(lhsMantissa);
    }
    if (rhs.bits.hi & 0x80000000) {
        rhsMantissa = Math_Neg64(rhsMantissa);
    }

    lhsMantissa = Math_Add64WithShift(lhsMantissa, 0, 9);
    rhsMantissa = Math_Add64WithShift(rhsMantissa, 0, 9);
    if (rhsExponent < lhsExponent) {
        rhsMantissa = Math_Add64WithShift(rhsMantissa, 1,
                                          lhsExponent - rhsExponent);
    } else {
        lhsMantissa = Math_Add64WithShift(lhsMantissa, 1,
                                          rhsExponent - lhsExponent);
        lhsExponent = rhsExponent;
    }

    lhsMantissa = Math_Add64(lhsMantissa, rhsMantissa);
    if ((int)lhsMantissa.hi < 0) {
        lhsMantissa = Math_Neg64(lhsMantissa);
        sign = 0x80000000;
    } else if (lhsMantissa.hi == 0 && lhsMantissa.lo == 0) {
        return output.value;
    }

    while ((lhsMantissa.hi & 0xe0000000) == 0) {
        lhsMantissa = Math_Add64WithShift(lhsMantissa, 0, 1);
        lhsExponent--;
    }
    if (lhsMantissa.hi & 0x40000000) {
        lhsMantissa = Math_Add64WithShift(lhsMantissa, 1, 1);
        lhsExponent++;
    }

    rounding.hi = 0;
    if (lhsMantissa.lo & 0x200) {
        rounding.lo = 0x100;
    } else {
        rounding.lo = 0xff;
    }
    lhsMantissa = Math_Add64(lhsMantissa, rounding);
    if (lhsMantissa.hi & 0x40000000) {
        lhsMantissa = Math_Add64WithShift(lhsMantissa, 1, 1);
        lhsExponent++;
    }
    lhsMantissa = Math_Add64WithShift(lhsMantissa, 1, 9);
    lhsMantissa.hi &= 0xffefffff;

    if (lhsExponent >= 0x7ff) {
        Evt_Deliver(0x22, 0xb);
        output.bits.lo = 0;
        output.bits.hi = sign == 0 ? 0x7ff00000 : 0xfff00000;
        return output.value;
    }

    output.bits.lo = lhsMantissa.lo;
    output.bits.hi = sign | (lhsExponent << 20) | lhsMantissa.hi;
    return output.value;
}
