#include "pe1/floor_clip.h"
/* CC1_FLAGS: -G8 -fno-strength-reduce */
/* MASPSX_FLAGS: -G4 --expand-div */

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
    int edgeId;
    u32 nextIndex;
    u16 nextX, nextZ;

    u16 *baseFlat;
    int pxFlat;
    int pzFlat;
    u16 *baseSlope;
    int pxSlope;
    int pzSlope;
    u16 *wordSlope;
    u32 slotSlope;
    u16 *wordFlat;
    u32 slotFlat;

    neighbours = D_8009CD88;
    queryX = x;
    queryZ = z;
    if (!D_8009D1D8) {
        CollisionVertexTable vertex;
        register int work asm("$16");
        baseFlat = triangle;
        pxFlat = x;
        pzFlat = z;
        wordFlat = baseFlat;
        slotFlat = 0;

        nextIndex = wordFlat[3];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
        nextX = vertex.xz->x;
        nextZ = vertex.xz->z;
        do {
            u32 prevIndex = nextIndex;
            register u16 prevX asm("$22") = nextX;
            u16 prevZ = nextZ;
            u16 edge = wordFlat[4];
            u32 *bits = &D_8009DFB0[edge >> 5];
            u32 bit = 1 << (edge & 0x1F);

            asm("" : : "r"(bits), "r"(bit));
            nextIndex = wordFlat[1];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
            nextX = vertex.xz->x;
            nextZ = vertex.xz->z;
            if (!(*bits & bit)) {

                *bits |= bit;
                work = pxFlat - D_8009CE2C;
                if ((s16)nextX < work && (s16)prevX < work)
                    goto next;
                work = pxFlat + D_8009CE2C;
                if (work < (s16)nextX && work < (s16)prevX)
                    goto next;
                work = pzFlat - D_8009CE2C;
                if ((s16)nextZ < work && (s16)prevZ < work)
                    goto next;
                work = pzFlat + D_8009CE2C;
                if (work < (s16)nextZ && work < (s16)prevZ)
                    goto next;
                {
                    int dz = pzFlat - (s16)nextZ;
                    int dx = pxFlat - (s16)nextX;

                    edgeId = wordFlat[4];

                    work = ((FloorEdge *)&D_8009CE14[edgeId])->ramp.length;
                    work = (dz * ((s16)prevX - (s16)nextX)
                                - dx * ((s16)prevZ - (s16)nextZ)) / work;
                    if (work < 0)
                        work = -work;
                    if (work > D_8009CE2C)
                        goto next;
                    if (nextIndex < prevIndex) {
                        work = Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionX, dx << 16)
                              + Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionZ, dz << 16);
                        if (work < 0) {
                            work = dx * dx + dz * dz;
                            if (work > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                        if (((FloorEdge *)&D_8009CE14[edgeId])->length < work) {
                            int ex = pxFlat - (s16)prevX;
                            int ez = pzFlat - (s16)prevZ;

                            if (ex * ex + ez * ez > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                    } else {
                        int ex = pxFlat - (s16)prevX;
                        int ez = pzFlat - (s16)prevZ;

                        work = Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionX, ex << 16)
                              + Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionZ, ez << 16);
                        if (work < 0) {
                            work = ex * ex + ez * ez;
                            if (work > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                        if (((FloorEdge *)&D_8009CE14[edgeId])->length < work) {
                            if (dx * dx + dz * dz > D_8009CE2C * D_8009CE2C)
                                goto next;
                        }
                    }
                    {
                        u16 neighbour = wordFlat[7];

                        if (neighbour == 0xFFFF
                            || (((CollisionFace *)D_8009D1FC->triangles.xz[neighbour].words)->kind & 0x80)) {
                            D_8009CE0C.point.x = nextX;
                            D_8009CE0C.point.z = nextZ;
                            D_8009CE10.point.x = prevX;
                            D_8009CE10.point.z = prevZ;
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
                        *(s16 *)(slotFlat + (u8 *)neighbours.index) = neighbour;
                    }
                }
            }
next:
            wordFlat++;
            slotFlat += 2;
        } while (wordFlat < baseFlat + 3);
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
        CollisionVertexTable vertex;
        register int work asm("$16");
        baseSlope = triangle;
        pxSlope = x;
        pzSlope = z;
        wordSlope = baseSlope;
        slotSlope = 0;

        nextIndex = wordSlope[6];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
        do {
            u32 prevIndex = nextIndex;
            register u16 prevX asm("$22") = nextX;
            u16 prevZ = nextZ;
            u16 edge = wordSlope[7];
            u32 *bits = &D_8009DFB0[edge >> 5];
            u32 bit = 1 << (edge & 0x1F);

            asm("" : : "r"(bits), "r"(bit));
            nextIndex = wordSlope[4];
            COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
            nextX = vertex.xyz->x;
            nextZ = vertex.xyz->z;
            if (!(*bits & bit)) {

                *bits |= bit;
                work = pxSlope - D_8009CE2C;
                if ((s16)nextX < work && (s16)prevX < work)
                    goto next2;
                work = pxSlope + D_8009CE2C;
                if (work < (s16)nextX && work < (s16)prevX)
                    goto next2;
                work = pzSlope - D_8009CE2C;
                if ((s16)nextZ < work && (s16)prevZ < work)
                    goto next2;
                work = pzSlope + D_8009CE2C;
                if (work < (s16)nextZ && work < (s16)prevZ)
                    goto next2;
                {
                    int dz = pzSlope - (s16)nextZ;
                    int dx = pxSlope - (s16)nextX;

                    edgeId = wordSlope[7];

                    work = ((FloorEdge *)&D_8009CE14[edgeId])->ramp.length;
                    work = (dz * ((s16)prevX - (s16)nextX)
                                - dx * ((s16)prevZ - (s16)nextZ)) / work;
                    if (work < 0)
                        work = -work;
                    if (work > D_8009CE2C)
                        goto next2;
                    if (nextIndex < prevIndex) {
                        work = Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionX, dx << 16)
                              + Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionZ, dz << 16);
                        if (work < 0) {
                            work = dx * dx + dz * dz;
                            if (work > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                        if (((FloorEdge *)&D_8009CE14[edgeId])->length < work) {
                            int ex = pxSlope - (s16)prevX;
                            int ez = pzSlope - (s16)prevZ;

                            if (ex * ex + ez * ez > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                    } else {
                        int ex = pxSlope - (s16)prevX;
                        int ez = pzSlope - (s16)prevZ;

                        work = Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionX, ex << 16)
                              + Math_FixedMul(((FloorEdge *)&D_8009CE14[edgeId])->ramp.directionZ, ez << 16);
                        if (work < 0) {
                            work = ex * ex + ez * ez;
                            if (work > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                        if (((FloorEdge *)&D_8009CE14[edgeId])->length < work) {
                            if (dx * dx + dz * dz > D_8009CE2C * D_8009CE2C)
                                goto next2;
                        }
                    }
                    {
                        u16 neighbour = wordSlope[10];

                        if (neighbour == 0xFFFF
                            || (((CollisionFace *)D_8009D1FC->triangles.xyz[neighbour].words)->kind & 0x80)) {
                            D_8009CE0C.point.x = nextX;
                            D_8009CE0C.point.z = nextZ;
                            D_8009CE10.point.x = prevX;
                            D_8009CE10.point.z = prevZ;
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
                        *(s16 *)(slotSlope + (u8 *)neighbours.index) = neighbour;
                    }
                }
            }
next2:
            wordSlope++;
            slotSlope += 2;
        } while (wordSlope < baseSlope + 3);
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
