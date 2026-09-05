#include "pe1/math64.h"

int Math_Cmp64Pair(MathU64 left, MathU64 right)
{
    if (left.hi > right.hi) goto greater;
    if (left.hi < right.hi) return -1;
    if (left.lo > right.lo) return 1;
    {
        register int less asm("$3") = left.lo < right.lo;
        if (less) return -1;
    }
    asm("");
    return 0;
greater:
    {
        register int result asm("$2") = 1;
        asm("" : : "r"(result));
        return result;
    }
}
