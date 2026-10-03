#include "pe1/field_collision.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

int Geo_PointInTri(void *triangle, s16 x, s16 z)
{
    u16 *indices = triangle;
    u16 nextX, nextZ, previousX, previousZ;
    unsigned edge;
    unsigned inside;

    if (D_8009D1D8) {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, indices[6], CollisionVertexXYZ);
        nextZ = vertex.xyz->z;
        nextX = vertex.xyz->x;
    } else {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, indices[3], CollisionVertexXZ);
        nextZ = vertex.xz->z;
        nextX = vertex.xz->x;
    }

    edge = 0;
    inside = 0;
    while (edge < 3) {
        previousX = nextX;
        previousZ = nextZ;
        if (D_8009D1D8) {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, indices[edge + 4], CollisionVertexXYZ);
            nextZ = vertex.xyz->z;
            nextX = vertex.xyz->x;
        } else {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, indices[edge + 1], CollisionVertexXZ);
            nextZ = vertex.xz->z;
            nextX = vertex.xz->x;
        }

        if ((z >= (s16)nextZ && z < (s16)previousZ) ||
            (z < (s16)nextZ && z >= (s16)previousZ)) {
            if (x < (s16)nextX && x < (s16)previousX) {
                inside = !inside;
            } else if (x < (s16)nextX || x < (s16)previousX) {
                int deltaZ = (s16)previousZ - (s16)nextZ;
                int side = ((s16)previousX - (s16)nextX) * (z - (s16)nextZ);

                if (deltaZ < 0) {
                    deltaZ *= x - (s16)nextX;
                    if (side < deltaZ)
                        inside = !inside;
                } else {
                    deltaZ *= x - (s16)nextX;
                    if (deltaZ < side)
                        inside = !inside;
                }
            }
        }
        ++edge;
    }
    return inside;
}
