/* GCC_VERSION: 2.8.1 */
#include "pe1/math64.h"

MathU64 *Math_Add64(MathU64 *result, MathU64 left, MathU64 right)
{
    unsigned int parts[4];

    parts[1] = left.lo >> 16;
    parts[0] = left.lo & 0xFFFF;
    parts[3] = right.lo >> 16;
    parts[2] = right.lo & 0xFFFF;
    parts[0] += parts[2];
    if (parts[0] & 0x10000) {
        parts[1]++;
    }
    parts[1] += parts[3];
    if (parts[1] & 0x10000) {
        left.hi++;
    }
    left.lo = (parts[1] << 16) | (parts[0] & 0xFFFF);
    left.hi += right.hi;
    *(unsigned long long *)result = *(unsigned long long *)&left;
        return result;
}
