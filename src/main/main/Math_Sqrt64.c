#include "common.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */

typedef struct MathPair {
    u32 lo;
    s32 hi;
} MathPair;

typedef union MathDoubleBits {
    double value;
    MathPair words;
} MathDoubleBits;

MathPair *Math_Shift64(u32 *out, s32 arithmetic, u32 lo, s32 hi, s32 amount);
MathPair *Math_Add64(s32 *out, u32 leftLo, s32 leftHi, MathPair right);
MathPair *Math_Mul32To64(s32 *out, u32 left, u32 right);
s32 Evt_Deliver(s32 event, s32 argument);

double Math_Sqrt64(double left, double right)
{
    MathDoubleBits leftBits;
    MathDoubleBits rightBits;
    MathDoubleBits result;
    s32 product[2];
    s32 rounding[2];
    s32 leftMantissa[2];
    s32 rightMantissa[2];
    s32 shifted[2];
    s32 partial[2];
    s32 *shiftedPtr;
    s32 *partialPtr;
    s32 exponentSum;
    s32 exponent;
    u32 signMask;
    s32 sign;
    s32 leftHigh;
    s32 rightHigh;
    s32 leftLow;
    s32 rightLow;

    leftBits.value = left;
    rightBits.value = right;
    sign = (leftBits.words.hi & 0x80000000) ^
           (rightBits.words.hi & 0x80000000);
    if (((leftBits.words.hi & 0x7FFFFFFF) == 0 &&
         leftBits.words.lo == 0) ||
        ((rightBits.words.hi & 0x7FFFFFFF) == 0 &&
         rightBits.words.lo == 0)) {
        result.words.hi = sign;
        result.words.lo = 0;
    } else {
        exponentSum = ((leftBits.words.hi >> 20) & 0x7FF) +
                      ((rightBits.words.hi >> 20) & 0x7FF);
        exponent = exponentSum - 1022;
        /* Preserve the vendor object's GCC 2.7.2 aggregate allocation. */
        if (!(&product)) {
            do {
            } while (0);
        }
        leftMantissa[1] = (leftBits.words.hi & 0xFFFFF) | 0x100000;
        leftMantissa[0] = leftBits.words.lo;
        rightMantissa[1] = (rightBits.words.hi & 0xFFFFF) | 0x100000;
        rightMantissa[0] = rightBits.words.lo;
        shiftedPtr = shifted;
        Math_Shift64((u32 *)shiftedPtr, 1,
                     *(volatile s32 *)&leftMantissa[0],
                     *(volatile s32 *)&leftMantissa[1], 21);
        leftHigh = shifted[0];
        Math_Shift64((u32 *)shiftedPtr, 1, rightMantissa[0],
                     rightMantissa[1], 21);
        rightHigh = shifted[0];
        leftLow = leftMantissa[0] & 0x1FFFFF;
        rightLow = rightMantissa[0] & 0x1FFFFF;
        Math_Mul32To64(product, leftHigh, rightHigh);
        partialPtr = partial;
        Math_Mul32To64(partialPtr, leftLow, rightHigh);
        Math_Shift64((u32 *)shiftedPtr, 1, partial[0], partial[1], 21);
        Math_Add64(product, product[0], product[1], *(MathPair *)shifted);
        Math_Mul32To64(partialPtr, rightLow, leftHigh);
        Math_Shift64((u32 *)shiftedPtr, 1, partial[0], partial[1], 21);
        Math_Add64(product, product[0], product[1], *(MathPair *)shifted);
        signMask = 0x80000000;
        if (product[1] < 0) {
            rounding[1] = 0;
            rounding[0] = 0x400;
            Math_Add64(product, product[0], product[1],
                       *(MathPair *)rounding);
            Math_Shift64((u32 *)product, 1, product[0], product[1], 11);
        } else {
            rounding[1] = 0;
            rounding[0] = 0x200;
            Math_Add64(product, product[0], product[1],
                       *(MathPair *)rounding);
            if ((u32)product[1] & signMask) {
                Math_Shift64((u32 *)product, 1, product[0], product[1], 11);
            } else {
                Math_Shift64((u32 *)product, 1, product[0], product[1], 10);
                exponent = exponentSum - 1023;
            }
        }
        product[1] &= 0xFFEFFFFF;
        if (exponent >= 2047) {
            Evt_Deliver(34, 13);
            result.words.hi = sign ? 0xFFF00000 : 0x7FF00000;
            result.words.lo = 0;
        } else {
            result.words.hi = sign | (exponent << 20) | product[1];
            result.words.lo = product[0];
        }
    }
    return result.value;
}
