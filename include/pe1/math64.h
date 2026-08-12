#ifndef PE1_MATH64_H
#define PE1_MATH64_H

/* Little-endian 64-bit intermediate used by the software math helpers. */
typedef struct MathU64 {
    unsigned int lo;
    unsigned int hi;
} MathU64;

typedef union MathDoubleBits {
    double value;
    MathU64 bits;
} MathDoubleBits;

MathU64 Math_Add64(MathU64 left, MathU64 right);
MathU64 Math_Add64WithShift(int arithmetic, MathU64 value, int amount);
int Math_Cmp64Pair(MathU64 left, MathU64 right);
MathU64 Math_Shift64(int arithmetic, MathU64 value, int amount);
MathU64 Math_Neg64(MathU64 value);
double Math_Int32ToDouble(int value);
int Math_DoubleToInt32(double value);
double Math_Div64(double numerator, double denominator);
MathU64 Math_Mul32To64(unsigned int left, unsigned int right);
double Math_Mul64(double left, double right);
double Math_Sqrt64(double left, double right);

#endif
