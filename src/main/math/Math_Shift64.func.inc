#include "pe1/math64.h"

MathU64 MATH_SHIFT64_NAME(int right, MathU64 value, int amount)
{
    int i;

    if (!right) {
        for (i = 0; i < amount; i++) {
            value.hi <<= 1;
            if (value.lo & 0x80000000) {
                value.hi |= 1;
            }
            value.lo <<= 1;
        }
    } else {
        for (i = 0; i < amount; i++) {
            value.lo >>= 1;
            if (value.hi & 1) {
                value.lo |= 0x80000000;
            }
            value.hi = MATH_SHIFT64_HIGH(value.hi);
        }
    }
    return value;
}

#undef MATH_SHIFT64_NAME
#undef MATH_SHIFT64_HIGH
