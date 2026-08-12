#include "pe1/math64.h"

int Math_Cmp64Pair(MathU64 left, MathU64 right)
{
    if (left.hi > right.hi) {
        return 1;
    }
    if (left.hi < right.hi) {
        return -1;
    }
    if (left.lo > right.lo) {
        return 1;
    }
    if (left.lo < right.lo) {
        return -1;
    }
    return 0;
}
