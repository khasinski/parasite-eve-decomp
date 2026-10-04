#include "pe1/floor_clip.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 */

/* Tests the circle of radius D_8009CE2C around (x, z) against the edges of
 * a walkable triangle. An edge the circle touches that has no walkable
 * neighbour is published as the blocking boundary (D_8009CE0C..D_8009CE28,
 * edge id in D_8009CE18) and 0 is returned; touched edges with a neighbour
 * queue that triangle, which is then walked recursively. Every edge is
 * tested once per query (D_8009DFB0 bitmap). Returns 1 when nothing
 * blocks. */
int Geo_ClipToFloorBoundary(s16 x, s16 z, u16 *triangle)
{
    FloorNeighbours neighbours;
    s16 queryX, queryZ;

    neighbours = D_8009CD88;
    queryX = x;
    queryZ = z;
    if (!D_8009D1D8) {
        int px = x;
        int pz = z;
        u16 *base = triangle;
        u16 *word = base;
        int slot = 0;
        u32 nextIndex;
        u16 nextX, nextZ;
        CollisionVertexTable vertex;

        nextIndex = word[3];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
        nextX = vertex.xz->x;
        nextZ = vertex.xz->z;
        do {
            u32 prevIndex = nextIndex;
            u16 prevX = nextX;
            u16 prevZ = nextZ;
            u16 edge = word[4];
            u32 *bits = &D_8009DFB0[edge >> 5];
            u32 bit = 1 << (edge & 0x1F);

            nextIndex = word[1];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
            nextX = vertex.xz->x;
            nextZ = vertex.xz->z;
            if (!(*bits & bit)) {
                int limit;

                *bits |= bit;
                limit = px - D_8009CE2C;
                if ((s16)nextX < limit && (s16)prevX < limit)
                    goto next;
                limit = px + D_8009CE2C;
                if (limit < (s16)nextX && limit < (s16)prevX)
                    goto next;
                limit = pz - D_8009CE2C;
                if ((s16)nextZ < limit && (s16)prevZ < limit)
                    goto next;
                limit = pz + D_8009CE2C;
                if (limit < (s16)nextZ && limit < (s16)prevZ)
                    goto next;
                {
                    int dz = pz - (s16)nextZ;
                    int dx = px - (s16)nextX;
                    int edgeId = word[4];
                    FloorEdge *record = (FloorEdge *)&D_8009CE14[edgeId];
                    int distance;
                    int along;

                    distance = (dz * ((s16)prevX - (s16)nextX)
                                - dx * ((s16)prevZ - (s16)nextZ)) / record->ramp.length;
                    if (distance < 0)
                        distance = -distance;
                    if (distance > D_8009CE2C)
                        goto next;
                    if (nextIndex < prevIndex) {
                        along = Math_FixedMul(record->ramp.directionX, dx << 16)
                              + Math_FixedMul(record->ramp.directionZ, dz << 16);
                        if (along < 0) {
                            along = dx * dx + dz * dz;
                            if (along > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                        if (record->length < along) {
                            int ex = px - (s16)prevX;
                            int ez = pz - (s16)prevZ;

                            if (ex * ex + ez * ez > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                    } else {
                        int ex = px - (s16)prevX;
                        int ez = pz - (s16)prevZ;

                        along = Math_FixedMul(record->ramp.directionX, ex << 16)
                              + Math_FixedMul(record->ramp.directionZ, ez << 16);
                        if (along < 0) {
                            along = ex * ex + ez * ez;
                            if (along > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                        if (record->length < along) {
                            if (dx * dx + dz * dz > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                    }
                    {
                        u16 neighbour = word[7];

                        if (neighbour == 0xFFFF
                            || (D_8009D1FC->triangles.xz[neighbour].words[0] & 0x80)) {
                            D_8009CE0C = nextX;
                            D_8009CE0E = nextZ;
                            D_8009CE10 = prevX;
                            D_8009CE12 = prevZ;
                            if ((s16)prevX < (s16)nextX) {
                                D_8009CE1C = nextX;
                                D_8009CE20 = prevX;
                            } else {
                                D_8009CE1C = prevX;
                                D_8009CE20 = nextX;
                            }
                            if ((s16)prevZ < (s16)nextZ) {
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
            }
next:
            word++;
            slot++;
        } while (word < base + 3);
        {
            u32 i;
            s16 *index = neighbours.index;

            for (i = 0; i < 3; i++, index++) {
                if (*index >= 0
                    && !Geo_ClipToFloorBoundary(queryX, queryZ,
                                                D_8009D1FC->triangles.xz[*index].words))
                    return 0;
            }
            return 1;
        }
    } else {
        int px = x;
        int pz = z;
        u16 *base = triangle;
        u16 *word = base;
        int slot = 0;
        u32 nextIndex;
        u16 nextX, nextZ;
        CollisionVertexTable vertex;

        nextIndex = word[6];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
        do {
            u32 prevIndex = nextIndex;
            u16 prevX = nextX;
            u16 prevZ = nextZ;
            u16 edge = word[7];
            u32 *bits = &D_8009DFB0[edge >> 5];
            u32 bit = 1 << (edge & 0x1F);

            nextIndex = word[4];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
            nextX = vertex.xyz->x;
            nextZ = vertex.xyz->z;
            if (!(*bits & bit)) {
                int limit;

                *bits |= bit;
                limit = px - D_8009CE2C;
                if ((s16)nextX < limit && (s16)prevX < limit)
                    goto next2;
                limit = px + D_8009CE2C;
                if (limit < (s16)nextX && limit < (s16)prevX)
                    goto next2;
                limit = pz - D_8009CE2C;
                if ((s16)nextZ < limit && (s16)prevZ < limit)
                    goto next2;
                limit = pz + D_8009CE2C;
                if (limit < (s16)nextZ && limit < (s16)prevZ)
                    goto next2;
                {
                    int dz = pz - (s16)nextZ;
                    int dx = px - (s16)nextX;
                    int edgeId = word[7];
                    FloorEdge *record = (FloorEdge *)&D_8009CE14[edgeId];
                    int distance;
                    int along;

                    distance = (dz * ((s16)prevX - (s16)nextX)
                                - dx * ((s16)prevZ - (s16)nextZ)) / record->ramp.length;
                    if (distance < 0)
                        distance = -distance;
                    if (distance > D_8009CE2C)
                        goto next2;
                    if (nextIndex < prevIndex) {
                        along = Math_FixedMul(record->ramp.directionX, dx << 16)
                              + Math_FixedMul(record->ramp.directionZ, dz << 16);
                        if (along < 0) {
                            along = dx * dx + dz * dz;
                            if (along > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                        if (record->length < along) {
                            int ex = px - (s16)prevX;
                            int ez = pz - (s16)prevZ;

                            if (ex * ex + ez * ez > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                    } else {
                        int ex = px - (s16)prevX;
                        int ez = pz - (s16)prevZ;

                        along = Math_FixedMul(record->ramp.directionX, ex << 16)
                              + Math_FixedMul(record->ramp.directionZ, ez << 16);
                        if (along < 0) {
                            along = ex * ex + ez * ez;
                            if (along > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                        if (record->length < along) {
                            if (dx * dx + dz * dz > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                    }
                    {
                        u16 neighbour = word[10];

                        if (neighbour == 0xFFFF
                            || (D_8009D1FC->triangles.xyz[neighbour].words[0] & 0x80)) {
                            D_8009CE0C = nextX;
                            D_8009CE0E = nextZ;
                            D_8009CE10 = prevX;
                            D_8009CE12 = prevZ;
                            if ((s16)prevX < (s16)nextX) {
                                D_8009CE1C = nextX;
                                D_8009CE20 = prevX;
                            } else {
                                D_8009CE1C = prevX;
                                D_8009CE20 = nextX;
                            }
                            if ((s16)prevZ < (s16)nextZ) {
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
            }
next2:
            word++;
            slot++;
        } while (word < base + 3);
        {
            u32 i;
            s16 *index = neighbours.index;

            for (i = 0; i < 3; i++, index++) {
                if (*index >= 0
                    && !Geo_ClipToFloorBoundary(queryX, queryZ,
                                                D_8009D1FC->triangles.xyz[*index].words))
                    return 0;
            }
            return 1;
        }
    }
}
