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

MathU64 *_add_mant_d(MathU64 *result, MathU64 left, MathU64 right);
MathU64 *_dbl_shift(MathU64 *result, int arithmetic, MathU64 value, int amount);
int _comp_mant(MathU64 left, MathU64 right);
MathU64 *_dbl_shift_us(MathU64 *result, int arithmetic, MathU64 value, int amount);
MathU64 *_mainasu(MathU64 *result, MathU64 value);
double __floatsidf(int value);
int __fixdfsi(double value);
double __divdf3(double numerator, double denominator);
MathU64 *_mul_mant_d(MathU64 *result, unsigned int left, unsigned int right);
double __adddf3(double left, double right);
double __muldf3(double left, double right);
int _err_math(int error, int operation);

#endif
