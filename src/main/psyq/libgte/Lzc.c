#include "pe1/gte.h"

int Lzc(int value) {
    int result;

    gte_ldlzcs(value);
    gte_getlzcr(result);
    return result;
}
