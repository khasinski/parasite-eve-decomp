/* GCC_VERSION: 2.8.1 */
#include "pe1/math64.h"

/* Normalize the magnitude, then pack the IEEE-754 exponent and fraction.
 * Keep the retail signed comparisons, including the nonterminating INT_MIN
 * normalization path. The zero-register binding and scheduling constraints
 * below emit no instructions and are included in debt. */
double Math_Int32ToDouble(int value) {
    register unsigned int sign = 0;
    register int exponent = 0x41D;
    register MathDoubleBits result asm("$16");
    MathU64 parts;

    if (value == 0) {
        register unsigned int zero asm("$0");
        /* Architectural zero; this empty output does not read uninitialized C storage. */
        asm volatile("" : "=r"(zero));
        result.bits.lo = zero;
        result.bits.hi = zero;
    } else {
        if (value < 0) {
            sign = 0x80000000;
            value = (unsigned int)0 - value;
        }
        while (value <= 0xFFFFFF) {
            value = (unsigned int)value << 4;
            exponent -= 4;
        }
        while (value <= 0x3FFFFFFF) {
            value = (unsigned int)value << 1;
            exponent--;
        }
        ((volatile MathU64 *)&parts)->hi = value;
        ((volatile MathU64 *)&parts)->lo = 0;
        Math_Shift64(&parts, 1, *(volatile MathU64 *)&parts, 10);
        {
            register unsigned int high;
            register unsigned int packed asm("$17");
            register unsigned int mask asm("$2") = 0xFFEF0000;
            high = ((volatile MathU64 *)&parts)->hi;
            asm volatile("" : "=r"(high), "=r"(mask) : "0"(high), "1"(mask));
            mask |= 0xFFFF;
            asm volatile("" : "=r"(high), "=r"(mask) : "0"(high), "1"(mask));
            high &= mask;
            packed = high | sign;
            mask = exponent << 20;
            packed |= mask;
            result.bits.hi = packed;
            result.bits.lo = parts.lo;
            parts.hi = high;
        }
    }
    return result.value;
}
