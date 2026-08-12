#include "pe1/math64.h"

MathU64 Math_Add64(MathU64 left, MathU64 right)
{
    unsigned int half[4];

    half[0] = left.lo & 0xFFFF;
    half[1] = left.lo >> 16;
    half[2] = right.lo & 0xFFFF;
    half[3] = right.lo >> 16;

    half[0] += half[2];
    if (half[0] & 0x10000) {
        half[1]++;
    }
    half[1] += half[3];
    if (half[1] & 0x10000) {
        left.hi++;
    }

    left.lo = (half[1] << 16) | (half[0] & 0xFFFF);
    left.hi += right.hi;
    return left;
}
