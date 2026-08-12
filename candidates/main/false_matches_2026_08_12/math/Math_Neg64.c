#include "pe1/math64.h"

MathU64 Math_Neg64(MathU64 value) {
    MathU64 one;

    one.lo = 1;
    one.hi = 0;
    value.lo = ~value.lo;
    value.hi = ~value.hi;
    value = Math_Add64(value, one);
    return value;
}
