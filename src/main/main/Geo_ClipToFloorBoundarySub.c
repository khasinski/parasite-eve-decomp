#include "pe1/field_collision.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Walks from the triangle `indices` across the edge crossed by the segment
 * (x0, z0)-(x1, z1) into neighbouring triangles, never stepping back into
 * `previous`, until one contains the start point. Returns that triangle or
 * 0 when the segment leaves the walkable mesh. */
void *Geo_ClipToFloorBoundarySub(u16 *indices, void *previous, s16 x0, s16 z0,
                                 s16 x1, s16 z1)
{
    int deltaX, deltaZ;
    s16 maxX, minX, maxZ, minZ;
    u16 nextX, nextZ, previousX, previousZ;
    u16 edge;

    deltaX = x1 - x0;
    deltaZ = z1 - z0;
    if (deltaX < 0) {
        maxX = x0;
        minX = x1;
    } else {
        maxX = x1;
        minX = x0;
    }
    if (deltaZ < 0) {
        maxZ = z0;
        minZ = z1;
    } else {
        maxZ = z1;
        minZ = z0;
    }

    if (D_8009D1D8) {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, indices[6], CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
    } else {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, indices[3], CollisionVertexXZ);
        nextX = vertex.xz->x;
        nextZ = vertex.xz->z;
    }

    for (edge = 0; edge < 3; edge++) {
        int edgeX, edgeZ, startX, startZ, denominator, along;
        void *neighbour;

        previousX = nextX;
        previousZ = nextZ;
        if (D_8009D1D8) {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, indices[edge + 4], CollisionVertexXYZ);
            nextX = vertex.xyz->x;
            nextZ = vertex.xyz->z;
        } else {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, indices[edge + 1], CollisionVertexXZ);
            nextX = vertex.xz->x;
            nextZ = vertex.xz->z;
        }

        edgeX = (s16)nextX - (s16)previousX;
        if (edgeX > 0) {
            if (maxX < (s16)previousX || minX >= (s16)nextX)
                continue;
        } else {
            if (maxX < (s16)nextX || minX >= (s16)previousX)
                continue;
        }
        edgeZ = (s16)nextZ - (s16)previousZ;
        if (edgeZ > 0) {
            if (maxZ < (s16)previousZ || minZ >= (s16)nextZ)
                continue;
        } else {
            if (maxZ < (s16)nextZ || minZ >= (s16)previousZ)
                continue;
        }

        startX = x0 - (s16)nextX;
        startZ = z0 - (s16)nextZ;
        along = edgeZ * startX - edgeX * startZ;
        denominator = deltaZ * edgeX - deltaX * edgeZ;
        if (denominator > 0) {
            if (along < 0 || denominator < along)
                continue;
        } else {
            if (along > 0 || along < denominator)
                continue;
        }
        along = deltaX * startZ - deltaZ * startX;
        if (denominator > 0) {
            if (along < 0 || denominator < along)
                continue;
        } else {
            if (along > 0 || along < denominator)
                continue;
        }
        if (denominator == 0)
            continue;

        if (!D_8009D1D8) {
            neighbour = &D_8009D1FC->triangles.xz[indices[edge + 7]];
            if (neighbour == previous)
                continue;
            if (Geo_PointInTri(neighbour, x0, z0))
                return neighbour;
            neighbour = Geo_ClipToFloorBoundarySub(neighbour, indices, x0, z0,
                                                   x1, z1);
            if (neighbour)
                return neighbour;
        } else {
            neighbour = &D_8009D1FC->triangles.xyz[indices[edge + 10]];
            if (neighbour == previous)
                continue;
            if (Geo_PointInTri(neighbour, x0, z0))
                return neighbour;
            neighbour = Geo_ClipToFloorBoundarySub(neighbour, indices, x0, z0,
                                                   x1, z1);
            if (neighbour)
                return neighbour;
        }
    }
    return 0;
}
