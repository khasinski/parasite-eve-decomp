#ifndef PE1_MATH64_H
#define PE1_MATH64_H

/* Little-endian 64-bit intermediate used by the software math helpers. */
typedef struct MathU64 {
    unsigned int lo;
    unsigned int hi;
} MathU64;

MathU64 Math_Add64(MathU64 left, MathU64 right);
MathU64 Math_Add64WithShift(MathU64 value, int arithmetic, int amount);
int Math_Cmp64Pair(MathU64 left, MathU64 right);
MathU64 Math_Shift64(MathU64 value, int arithmetic, int amount);
MathU64 Math_Neg64(MathU64 value);

#endif
