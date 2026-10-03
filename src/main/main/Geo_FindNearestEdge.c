/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/field_collision.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"

/* Returns the first polygon edge (index of its end vertex) whose segment
 * passes within D_8009CE2C of (x, z), or -1 when no edge is that close. */
int Geo_FindNearestEdge(int x, int z, PolygonVertex *vertices,
                        unsigned short count)
{
    GteVector edge;
    s16 pointX = x;
    s16 pointZ = z;
    s16 prevX = vertices[count - 1].x;
    s16 prevZ = vertices[count - 1].z;
    unsigned short i;

    for (i = 0; i < count; i++) {
        s16 startX, startZ;
        int radius, vertexX, vertexZ;
        int endX, endZ;
        int length, distance, offsetX, offsetZ;

        startX = prevX;
        startZ = prevZ;
        radius = D_8009CE2C;
        vertexX = vertices[i].x;
        prevX = vertexX;
        endX = prevX;
        vertexZ = vertices[i].z;
        prevZ = vertexZ;
        distance = pointX - radius;
        if (endX < distance && startX < distance)
            continue;
        distance = pointX + radius;
        if (distance < endX && distance < startX)
            continue;
        endZ = prevZ;
        distance = pointZ - radius;
        if (endZ < distance && startZ < distance)
            continue;
        distance = pointZ + radius;
        if (distance < endZ && distance < startZ)
            continue;
        edge.x = startX - endX;
        edge.y = 0;
        edge.z = startZ - endZ;
        length = Gte_ISqrt(edge.x * edge.x + edge.z * edge.z);
        offsetZ = pointZ - endZ;
        offsetX = pointX - endX;
        distance = (offsetZ * edge.x - offsetX * edge.z) / length;
        if (distance < 0)
            distance = -distance;
        if (distance > D_8009CE2C)
            continue;
        Gte_NormalizeVec(&edge, &edge);
        distance = Math_FixedMul(edge.x << 4, offsetX << 16);
        distance += Math_FixedMul(edge.z << 4, offsetZ << 16);
        if (distance < 0) {
            distance = offsetX * offsetX + offsetZ * offsetZ;
            if (distance > D_8009CE2C * D_8009CE2C)
                continue;
        }
        if (length < distance >> 16) {
            distance = (pointX - startX) * (pointX - startX) +
                       (pointZ - startZ) * (pointZ - startZ);
            if (distance > D_8009CE2C * D_8009CE2C)
                continue;
        }
        return i;
    }
    return -1;
}
