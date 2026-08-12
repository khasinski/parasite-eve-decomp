#include "pe1/math64.h"
#include "pe1/event.h"


int Math_DoubleToInt32(double value) {
    MathDoubleBits input;
    MathU64 mantissa;
    int exponent;
    int shift;
    int result;
    int shifted;
    unsigned int mantissaHigh;

    input.value = value;
    if ((input.bits.hi & 0x7fffffff) == 0 && input.bits.lo == 0) {
        return input.bits.hi & 0x80000000;
    }

    exponent = ((int)input.bits.hi >> 20) & 0x7ff;
    shift = exponent - 0x41d;
    if (shift > 0) {
        Evt_Deliver(0x22, 0x11);
        result = 0x7fffffff;
        if ((int)input.bits.hi < 0) {
            result = 0x80000000;
        }
        return result;
    }

    mantissaHigh = 0x100000 | (input.bits.hi & 0xfffff);
    mantissa.hi = mantissaHigh;
    mantissa.lo = input.bits.lo;
    mantissa = Math_Add64WithShift(0, mantissa, 10);
    shifted = mantissa.hi;
    if ((unsigned int)(exponent - 0x3fe) < 0x20 && shifted != 0) {
        shifted >>= -shift;
    } else {
        return 0;
    }
    result = shifted;
    if ((int)input.bits.hi < 0) {
        result = -result;
    }
    return result;
}
