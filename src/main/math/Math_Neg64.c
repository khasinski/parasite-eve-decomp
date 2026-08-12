#include "pe1/math64.h"
/* MASPSX_FLAGS: --stack-return-delay */

MathU64 Math_Neg64(MathU64 value) {
    MathU64 one;

    one.hi = 0;
    one.lo = 1;
    value.hi = ~value.hi;
    value.lo = ~value.lo;
    value = Math_Add64(value, one);
    return value;
}
