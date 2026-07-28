#include "pe1/gte.h"

int AverageZ4(int z0, int z1, int z2, int z3) {
    int result;

    gte_ldsz4(z0, z1, z2, z3);
    gte_avsz4();
    gte_getotz(result);
    return result;
}
