#include "pe1/field_collision.h"

int Geo_PointInPoly(int x, int z, const PolygonVertex *vertices, unsigned short count)
{
    unsigned short i;
    /* Matching debt: the accumulator and edge-test result need distinct
     * register assignments on stock GCC 2.7.2. No instruction ASM. */
    register int inside asm("$11");
    int previousX, previousZ, currentX, currentZ;
    int cross, dz;
    register int intersects asm("$2");
    x >>= 16;
    z >>= 16;
    i = 0;
    inside = 0;
    currentX = vertices[count - 1].x;
    currentZ = vertices[count - 1].z;
    for (;;) {
        previousX = currentX;
        previousZ = currentZ;
        currentZ = vertices[i].z;
        currentX = vertices[i].x;
        if ((z >= currentZ && z < previousZ) ||
            (z < currentZ && z >= previousZ)) {
            if (x < currentX && x < previousX) {
                inside = !inside;
            } else if (x < currentX || x < previousX) {
                /* Keep the low 32 bits, as the retail MULT/MFLO does,
                 * without signed multiplication overflow in C. */
                cross = (int)((long long)(previousX - currentX) * (z - currentZ));
                dz = previousZ - currentZ;
                if (dz < 0) {
                    dz = (int)((long long)dz * (x - currentX));
                    intersects = cross < dz;
                } else {
                    dz = (int)((long long)dz * (x - currentX));
                    intersects = dz < cross;
                }
                if (intersects)
                    inside = !inside;
            }
        }
        ++i;
        if (i >= count)
            break;
    }
    return inside;
}
