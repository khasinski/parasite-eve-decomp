/* GCC_VERSION: 2.8.1 */
#include "pe1/math64.h"

/* Four 16-bit partial products, with the two cross terms accumulated by
 * Math_Add64. The output pointer is passed in a0 and returned in v0.
 * Register pins and empty scheduling barriers are included in debt. */
MathU64 *Math_Mul32To64(MathU64 *out, unsigned int left, unsigned int right) {
    volatile MathU64 result;
    MathU64 part;
    register unsigned int low asm("$18") = left & 0xFFFF;
    register unsigned int rightLow asm("$2");
    register unsigned int firstProduct;
    register unsigned int product asm("$2");
    register MathU64 *resultPtr;

    asm("" : "=r"(low), "=r"(right) : "0"(low), "1"(right));
    rightLow = right & 0xFFFF;
    firstProduct = low * rightLow;
    asm("" : "=r"(firstProduct) : "0"(firstProduct), "r"(low));
    left >>= 16;
    product = left * rightLow;
    resultPtr = (MathU64 *)&result;
    asm("" : "=r"(resultPtr) : "0"(resultPtr));
    result.hi = 0;
    result.lo = firstProduct;
    part.hi = product >> 16;
    part.lo = product << 16;
    asm("" : "+m"(part.lo) : "m"(part.hi));
    right >>= 16;
    Math_Add64(resultPtr, result, part);

    product = low * right;
    part.hi = product >> 16;
    part.lo = product << 16;
    asm("" : "+m"(part.lo) : "m"(part.hi));
    Math_Add64(&result, result, part);
    {
        register unsigned int last asm("$3") = left * right;
        register unsigned int hi = result.hi;
        hi += last;
        result.hi = hi;
    }
    {
        register unsigned int lo;
        register unsigned int hi;
        lo = result.lo;
        hi = result.hi;
        out->lo = lo;
        out->hi = hi;
    }
    asm volatile("" : : "m"(*out));
    return out;
}
