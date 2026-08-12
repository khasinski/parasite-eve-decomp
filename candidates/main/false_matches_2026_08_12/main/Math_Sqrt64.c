#include "pe1/event.h"
#include "pe1/math64.h"

double Math_Sqrt64(double left, double right) {
    MathDoubleBits lhs;
    MathDoubleBits rhs;
    MathDoubleBits output;
    MathU64 lhsMantissa;
    MathU64 rhsMantissa;
    MathU64 product;
    MathU64 partial;
    MathU64 rounding;
    unsigned int lhsUpper;
    unsigned int rhsUpper;
    unsigned int sign;
    int exponentSum;
    int exponent;

    lhs.value = left;
    rhs.value = right;
    sign = (lhs.bits.hi & 0x80000000) ^ (rhs.bits.hi & 0x80000000);
    if (((lhs.bits.hi & 0x7fffffff) == 0 && lhs.bits.lo == 0) ||
        ((rhs.bits.hi & 0x7fffffff) == 0 && rhs.bits.lo == 0)) {
        output.bits.lo = 0;
        output.bits.hi = sign;
        return output.value;
    }

    exponentSum = ((lhs.bits.hi >> 20) & 0x7ff) +
                  ((rhs.bits.hi >> 20) & 0x7ff);
    exponent = exponentSum - 0x3fe;

    lhsMantissa.lo = lhs.bits.lo;
    lhsMantissa.hi = (lhs.bits.hi & 0xfffff) | 0x100000;
    rhsMantissa.lo = rhs.bits.lo;
    rhsMantissa.hi = (rhs.bits.hi & 0xfffff) | 0x100000;
    lhsMantissa = Math_Shift64(1, lhsMantissa, 21);
    rhsMantissa = Math_Shift64(1, rhsMantissa, 21);

    lhsUpper = lhs.bits.lo & 0x1fffff;
    rhsUpper = rhs.bits.lo & 0x1fffff;
    product = Math_Mul32To64(lhsMantissa.lo, rhsMantissa.lo);

    partial = Math_Mul32To64(lhsUpper, rhsMantissa.lo);
    partial = Math_Shift64(1, partial, 21);
    product = Math_Add64(product, partial);

    partial = Math_Mul32To64(rhsUpper, lhsMantissa.lo);
    partial = Math_Shift64(1, partial, 21);
    product = Math_Add64(product, partial);

    rounding.hi = 0;
    if ((int)product.hi < 0) {
        rounding.lo = 0x400;
        product = Math_Add64(product, rounding);
        product = Math_Shift64(1, product, 11);
    } else {
        rounding.lo = 0x200;
        product = Math_Add64(product, rounding);
        if (product.hi & 0x80000000) {
            product = Math_Shift64(1, product, 11);
        } else {
            exponent = exponentSum - 0x3ff;
            product = Math_Shift64(1, product, 10);
        }
    }

    product.hi &= 0xffefffff;
    if (exponent >= 0x7ff) {
        Evt_Deliver(0x22, 0xd);
        output.bits.lo = 0;
        output.bits.hi = sign == 0 ? 0x7ff00000 : 0xfff00000;
        return output.value;
    }

    output.bits.lo = product.lo;
    output.bits.hi = sign | (exponent << 20) | product.hi;
    return output.value;
}
