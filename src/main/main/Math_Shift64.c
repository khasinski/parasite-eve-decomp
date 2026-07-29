typedef unsigned int u32;

#include "pe1/math64.h"

MathU64 *Math_Shift64(MathU64 *out, int direction, u32 lo, u32 hi, int amount) {
    int i;

    if (direction == 0) {
        for (i = 0; i < amount; i++) {
            hi <<= 1;
            if (lo & 0x80000000) {
                hi |= 1;
            }
            lo <<= 1;
        }
    } else {
        for (i = 0; i < amount; i++) {
            lo >>= 1;
            if (hi & 1) {
                lo |= 0x80000000;
            }
            hi >>= 1;
        }
    }

    out->lo = lo;
    out->hi = hi;
    return out;
}
