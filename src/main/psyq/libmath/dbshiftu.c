/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/math64.h"

MathU64 *Math_Shift64(MathU64 *result, int right, MathU64 value, int amount)
{
    register MathU64 *out asm("$8") = result;
    int shift = amount;
    int i;
    unsigned int lo;
    unsigned int hi;
    if (!right) {
        for (i = 0; i < shift; i++) {
            value.hi <<= 1;
            if (value.lo & 0x80000000) {
                value.hi |= 1;
            }
            value.lo <<= 1;
        }
    } else {
        for (i = 0; i < shift; i++) {
            value.lo >>= 1;
            if (value.hi & 1) {
                value.lo |= 0x80000000;
            }
            value.hi = (unsigned int)value.hi >> 1;
        }
    }

    lo = value.lo;
    hi = value.hi;
    out->lo = lo;
    out->hi = hi;
    __asm__ volatile("" ::: "memory");
    return out;
}
