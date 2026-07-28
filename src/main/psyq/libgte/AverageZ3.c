#include "pe1/gte.h"

int AverageZ3(int z1, int z2, int z3) {
    int result;

    gte_ldsz3(z1, z2, z3);
    gte_avsz3();
    gte_getotz(result);
    return result;
}
