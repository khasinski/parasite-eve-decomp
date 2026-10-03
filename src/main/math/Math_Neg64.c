/* GCC_VERSION: 2.8.1 */
#include "common.h"
#include "pe1/math64.h"

MathU64 *Math_Neg64(MathU64 *out, MathU64 value)
{
    MathU64 one;

    one.hi = 0;
    one.lo = 1;
    value.hi = ~value.hi;
    value.lo = ~value.lo;
    asm("" : "+m"(value.lo), "+m"(value.hi));
    asm("" : "+m"(one.lo));
    Math_Add64(&value, value, one);
    {
        u32 resultLo;
        s32 resultHi;

        resultLo = value.lo;
        resultHi = value.hi;
        out->lo = resultLo;
        out->hi = resultHi;
    }
    asm volatile("" : : "m"(*out));
    return out;
}
