#include "pe1/math64.h"

MathU64 Math_Add64(MathU64 left, MathU64 right) {
    unsigned int low;
    unsigned int high;
    unsigned int carryMask;

    carryMask = 0x10000;
    low = (left.lo & 0xffff) + (right.lo & 0xffff);
    high = left.lo >> 16;
    if (low & carryMask) {
        high++;
    }
    high += right.lo >> 16;
    if (high & carryMask) {
        left.hi++;
    }
    left.lo = (high << 16) | (low & 0xffff);
    left.hi += right.hi;
    return left;
}
