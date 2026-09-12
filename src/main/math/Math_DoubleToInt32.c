#include "common.h"
#include "pe1/math64.h"

/* GCC_VERSION: 2.8.1 */

void Evt_Deliver(s32 event, s32 argument);

s32 Math_DoubleToInt32(register double value)
{
    register MathDoubleBits input;
    volatile MathU64 parts;
    register s32 exponent;
    register s32 shift;
    s32 mantissa;
    s32 result;

    input.value = value;
    if (((input.bits.hi & 0x7FFFFFFF) == 0) && (input.bits.lo == 0)) {
        result = input.bits.hi & 0x80000000;
        goto done;
    }

    exponent = 20;
    exponent = ((s32)input.bits.hi >> exponent) & 0x7FF;
    shift = exponent - 0x41D;
    if (shift > 0) {
        Evt_Deliver(0x22, 0x11);
        result = 0x7FFFFFFF;
        if ((s32)input.bits.hi < 0) {
            result = 0x80000000;
        }
        goto done;
    }

    parts.hi = (input.bits.hi & 0xFFFFF) | 0x100000;
    parts.lo = input.bits.lo;
    Math_Add64WithShift(&parts, 0, parts, 10);
    mantissa = parts.hi;
    if ((u32)(exponent - 0x3FE) >= 0x20) goto zero;
    if (mantissa == 0) goto zero;
    mantissa >>= -shift;
    goto applySign;

zero:
    result = 0;
    goto done;
applySign:
    result = mantissa;
    if ((s32)input.bits.hi < 0) {
        result = -result;
    }
done:
    return result;
}
