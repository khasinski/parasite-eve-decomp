#include "common.h"
#include "pe1/math64.h"

/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

MathU64 Math_Mul32To64(u32 left, u32 right)
{
    MathU64 result;
    MathU64 partial;
    u32 leftLow;
    u32 leftHigh;
    u32 rightLow;
    u32 rightHigh;
    u32 lowProduct;
    u32 crossProduct;

    leftLow = left & 0xFFFF;
    leftHigh = left >> 16;
    rightLow = right & 0xFFFF;
    rightHigh = right >> 16;

    lowProduct = leftLow * rightLow;
    result.lo = lowProduct;
    result.hi = 0;

    crossProduct = leftHigh * rightLow;
    partial.hi = crossProduct >> 16;
    partial.lo = crossProduct << 16;
    result = Math_Add64(result, partial);

    crossProduct = leftLow * rightHigh;
    partial.hi = crossProduct >> 16;
    partial.lo = crossProduct << 16;
    result = Math_Add64(result, partial);

    result.hi += leftHigh * rightHigh;
    return result;
}
