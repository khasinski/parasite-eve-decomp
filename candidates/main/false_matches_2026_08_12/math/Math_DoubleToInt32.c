#include "pe1/math64.h"
#include "pe1/event.h"

int Math_DoubleToInt32(double value) {
    MathDoubleBits input;
    MathU64 mantissa;
    int exponent;
    int shift;
    int result;

    input.value = value;
    if ((input.bits.hi & 0x7fffffff) == 0 && input.bits.lo == 0) {
        return input.bits.hi & 0x80000000;
    }

    exponent = (input.bits.hi >> 20) & 0x7ff;
    shift = exponent - 0x41d;
    if (shift > 0) {
        Evt_Deliver(0x22, 0x11);
        if ((int)input.bits.hi >= 0) {
            return 0x7fffffff;
        }
        return 0x80000000;
    }

    mantissa.lo = input.bits.lo;
    mantissa.hi = (input.bits.hi & 0xfffff) | 0x100000;
    mantissa = Math_Add64WithShift(mantissa, 0, 10);
    if ((unsigned int)(exponent - 0x3fe) < 0x20 && mantissa.hi != 0) {
        result = (int)mantissa.hi >> -shift;
    } else {
        result = 0;
    }
    if ((int)input.bits.hi < 0) {
        result = -result;
    }
    return result;
}
