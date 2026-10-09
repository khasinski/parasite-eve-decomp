/* GCC_VERSION: 2.8.1 */
/* PSY-Q LIBMATH MAINASU: _mainasu. */
#include "common.h"
#include "pe1/math64.h"

MathU64 *_mainasu(MathU64 *out, MathU64 value)
{
    MathU64 one;

    one.hi = 0;
    one.lo = 1;
    value.hi = ~value.hi;
    value.lo = ~value.lo;
    asm("" : "+m"(value.lo), "+m"(value.hi), "+m"(one.lo));
    _add_mant_d(&value, value, one);
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
