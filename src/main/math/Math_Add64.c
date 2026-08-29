#include "common.h"
#include "pe1/math64.h"

/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

MathU64 Math_Add64(MathU64 left, MathU64 right)
{
    u32 parts[4];
    u32 leftHi16;
    u32 rightHi16;
    u32 lowSum;
    u32 highSum;
    u32 carryMask;

    leftHi16 = left.lo >> 16;
    parts[1] = leftHi16;
    parts[0] = left.lo & 0xFFFF;
    rightHi16 = right.lo >> 16;
    parts[3] = rightHi16;
    parts[2] = right.lo & 0xFFFF;

    lowSum = parts[0] + parts[2];
    parts[0] = lowSum;
    carryMask = 0x10000;
    if (lowSum & carryMask) {
        parts[1] = leftHi16 + 1;
    }

    highSum = parts[1] + parts[3];
    parts[1] = highSum;
    if (highSum & carryMask) {
        left.hi++;
    }

    left.lo = (highSum << 16) | (lowSum & 0xFFFF);
    left.hi += right.hi;
    return left;
}
