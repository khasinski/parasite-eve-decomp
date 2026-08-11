#include "pe1/math64.h"

int Math_Cmp64Pair(MathU64 left, MathU64 right) {
    if (right.hi < left.hi) {
        return 1;
    }
    if (left.hi < right.hi) {
        return -1;
    }
    if (right.lo < left.lo) {
        return 1;
    }
    if (left.lo < right.lo) {
        return -1;
    }
    return 0;
}
