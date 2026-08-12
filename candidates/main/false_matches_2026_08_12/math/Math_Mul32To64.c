#include "pe1/math64.h"

MathU64 Math_Mul32To64(unsigned int left, unsigned int right) {
    MathU64 result;
    MathU64 partial;
    unsigned int leftLow;
    unsigned int leftHigh;
    unsigned int rightLow;
    unsigned int rightHigh;
    unsigned int product;

    leftLow = left & 0xffff;
    rightLow = right & 0xffff;
    result.lo = leftLow * rightLow;
    result.hi = 0;

    leftHigh = left >> 16;
    product = leftHigh * rightLow;
    partial.lo = product << 16;
    partial.hi = product >> 16;
    result = Math_Add64(result, partial);

    rightHigh = right >> 16;
    product = leftLow * rightHigh;
    partial.lo = product << 16;
    partial.hi = product >> 16;
    result = Math_Add64(result, partial);

    result.hi += leftHigh * rightHigh;
    return result;
}
