/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 --expand-div --use-comm-section */
#include "pe1/floor_clip.h"

/* Tentative COMMON declarations preserve small-data metadata for stock
 * maspsx. Storage remains at the existing data/linker addresses. */
FloorEdgePoint D_8009CE0C;
u16 D_8009CE18;

/* Tests the circle of radius D_8009CE2C around (x, z) against the edges of
 * a walkable triangle (flat XZ format when there is no plane table, sloped
 * XYZ format otherwise). An edge the circle touches without a walkable
 * neighbour is published as the blocking boundary (D_8009CE0C..D_8009CE28,
 * edge id in D_8009CE18) and 0 is returned; touched edges with a neighbour
 * queue that triangle, which is then walked recursively. Every edge is
 * tested once per query (D_8009DFB0 bitmap). Returns 1 when nothing
 * blocks. */
int Geo_ClipToFloorBoundary(s16 x, s16 z, void *triangle)
{
    FloorNeighbours neighbours;
    int edgeId;
    u32 nextIndex;
    u16 nextX, nextZ;
    u32 prevIndex;
    u16 prevX, prevZ;
    u32 *bits;
    int d;

    neighbours = D_8009CD88;
    if (!D_8009D1D8) {
        u16 *tri = triangle;
        int px = x;
        int pz = z;
        u32 slot = 0;
        CollisionVertexTable vertex;

        nextIndex = tri[3];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
        nextX = vertex.xz->x;
        nextZ = vertex.xz->z;
        do {
            u32 bit = tri[slot + 4];

            prevIndex = nextIndex;
            prevX = nextX;
            prevZ = nextZ;
            bits = &D_8009DFB0[bit >> 5];
            bit = 1 << (bit & 0x1F);
            nextIndex = tri[slot + 1];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
            nextX = vertex.xz->x;
            nextZ = vertex.xz->z;
            if (!(*bits & bit)) {
                u16 r;
                int dx, dz;

                *bits |= bit;
                r = D_8009CE2C;
                d = px - r;
                if ((s16)nextX < d && (s16)prevX < d)
                    continue;
                d = px + r;
                if (d < (s16)nextX && d < (s16)prevX)
                    continue;
                d = pz - r;
                if ((s16)nextZ < d && (s16)prevZ < d)
                    continue;
                d = pz + r;
                if (d < (s16)nextZ && d < (s16)prevZ)
                    continue;
                dz = pz - (s16)nextZ;
                dx = px - (s16)nextX;
                edgeId = tri[slot + 4];
                d = D_8009CE14[edgeId].length.parts.integer;
                d = (dz * ((s16)prevX - (s16)nextX)
                     - dx * ((s16)prevZ - (s16)nextZ)) / d;
                if (d < 0)
                    d = -d;
                if (d > r)
                    continue;
                if (nextIndex < prevIndex) {
                    d = Math_FixedMul(D_8009CE14[edgeId].directionX, dx << 16);
                    d += Math_FixedMul(D_8009CE14[edgeId].directionZ, dz << 16);
                    if (d < 0) {
                        d = dx * dx + dz * dz;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                    if (D_8009CE14[edgeId].length.fixed < d) {
                        int ex = px - (s16)prevX;
                        int ez = pz - (s16)prevZ;

                        d = ex * ex + ez * ez;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                } else {
                    int ex = px - (s16)prevX;
                    int ez = pz - (s16)prevZ;

                    d = Math_FixedMul(D_8009CE14[edgeId].directionX, ex << 16);
                    d += Math_FixedMul(D_8009CE14[edgeId].directionZ, ez << 16);
                    if (d < 0) {
                        d = ex * ex + ez * ez;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                    if (D_8009CE14[edgeId].length.fixed < d) {
                        d = dx * dx + dz * dz;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                }
                {
                    u16 neighbour = tri[slot + 7];

                    if (neighbour == 0xFFFF
                        || (D_8009D1FC->triangles.xz[neighbour].kind & 0x80)) {
                        D_8009CE0C.point.x = nextX;
                        D_8009CE0C.point.z = nextZ;
                        D_8009CE10.point.x = prevX;
                        D_8009CE10.point.z = prevZ;
                        if ((s16)nextX > (s16)prevX) {
                            D_8009CE1C = nextX;
                            D_8009CE20 = prevX;
                        } else {
                            D_8009CE1C = prevX;
                            D_8009CE20 = nextX;
                        }
                        if ((s16)nextZ > (s16)prevZ) {
                            D_8009CE24 = nextZ;
                            D_8009CE28 = prevZ;
                        } else {
                            D_8009CE24 = prevZ;
                            D_8009CE28 = nextZ;
                        }
                        D_8009CE18 = edgeId;
                        return 0;
                    }
                    neighbours.index[slot] = neighbour;
                }
            }
        } while (++slot < 3);
        for (slot = 0; slot < 3; slot++) {
            if (neighbours.index[slot] >= 0
                && !Geo_ClipToFloorBoundary(
                       x, z, &D_8009D1FC->triangles.xz[neighbours.index[slot]]))
                return 0;
        }
        return 1;
    } else {
        u16 *tri = triangle;
        int px = x;
        int pz = z;
        u32 slot = 0;
        CollisionVertexTable vertex;

        nextIndex = tri[6];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
        do {
            u32 bit = tri[slot + 7];

            prevIndex = nextIndex;
            prevX = nextX;
            prevZ = nextZ;
            bits = &D_8009DFB0[bit >> 5];
            bit = 1 << (bit & 0x1F);
            nextIndex = tri[slot + 4];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
            nextX = vertex.xyz->x;
            nextZ = vertex.xyz->z;
            if (!(*bits & bit)) {
                u16 r;
                int dx, dz;

                *bits |= bit;
                r = D_8009CE2C;
                d = px - r;
                if ((s16)nextX < d && (s16)prevX < d)
                    continue;
                d = px + r;
                if (d < (s16)nextX && d < (s16)prevX)
                    continue;
                d = pz - r;
                if ((s16)nextZ < d && (s16)prevZ < d)
                    continue;
                d = pz + r;
                if (d < (s16)nextZ && d < (s16)prevZ)
                    continue;
                dz = pz - (s16)nextZ;
                dx = px - (s16)nextX;
                edgeId = tri[slot + 7];
                d = D_8009CE14[edgeId].length.parts.integer;
                d = (dz * ((s16)prevX - (s16)nextX)
                     - dx * ((s16)prevZ - (s16)nextZ)) / d;
                if (d < 0)
                    d = -d;
                if (d > r)
                    continue;
                if (nextIndex < prevIndex) {
                    d = Math_FixedMul(D_8009CE14[edgeId].directionX, dx << 16);
                    d += Math_FixedMul(D_8009CE14[edgeId].directionZ, dz << 16);
                    if (d < 0) {
                        d = dx * dx + dz * dz;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                    if (D_8009CE14[edgeId].length.fixed < d) {
                        int ex = px - (s16)prevX;
                        int ez = pz - (s16)prevZ;

                        d = ex * ex + ez * ez;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                } else {
                    int ex = px - (s16)prevX;
                    int ez = pz - (s16)prevZ;

                    d = Math_FixedMul(D_8009CE14[edgeId].directionX, ex << 16);
                    d += Math_FixedMul(D_8009CE14[edgeId].directionZ, ez << 16);
                    if (d < 0) {
                        d = ex * ex + ez * ez;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                    if (D_8009CE14[edgeId].length.fixed < d) {
                        d = dx * dx + dz * dz;
                        if (d > D_8009CE2C * D_8009CE2C)
                            continue;
                    }
                }
                {
                    u16 neighbour = tri[slot + 10];

                    if (neighbour == 0xFFFF
                        || (D_8009D1FC->triangles.xyz[neighbour].kind & 0x80)) {
                        D_8009CE0C.point.x = nextX;
                        D_8009CE0C.point.z = nextZ;
                        D_8009CE10.point.x = prevX;
                        D_8009CE10.point.z = prevZ;
                        if ((s16)nextX > (s16)prevX) {
                            D_8009CE1C = nextX;
                            D_8009CE20 = prevX;
                        } else {
                            D_8009CE1C = prevX;
                            D_8009CE20 = nextX;
                        }
                        if ((s16)nextZ > (s16)prevZ) {
                            D_8009CE24 = nextZ;
                            D_8009CE28 = prevZ;
                        } else {
                            D_8009CE24 = prevZ;
                            D_8009CE28 = nextZ;
                        }
                        D_8009CE18 = edgeId;
                        return 0;
                    }
                    neighbours.index[slot] = neighbour;
                }
            }
        } while (++slot < 3);
        for (slot = 0; slot < 3; slot++) {
            if (neighbours.index[slot] >= 0
                && !Geo_ClipToFloorBoundary(
                       x, z, &D_8009D1FC->triangles.xyz[neighbours.index[slot]]))
                return 0;
        }
        return 1;
    }
}
