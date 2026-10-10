/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 --expand-div --use-comm-section */
#include "common.h"
#include "pe1/field_collision.h"
#include "pe1/battle.h"
#include "pe1/entity_floor.h"
#include "pe1/gte.h"
#include "pe1/floor_clip.h"
#include "pe1/field_movement.h"


int Math_FixedMul(int a, int b);

void Entity_FindFloor(BattleEntity *actor)
{
    u8 *floor_data;
    u8 *simple_triangle;
    CollisionFace *complex_triangle;
    u16 *initial_indices;
    u16 *indices;
    u16 triangle_count;
    u32 floor_index;
    u32 triangle_index;
    u16 triangle_id;
    int floor_value;
    s16 x;
    s16 z;
    int x_height;
    int z_height;
    int floor_offset;
    u32 floor_table_offset;
    u8 scratch[8];

    if ((actor->entityFlags & 0x80) != 0) {
        return;
    }

    x = (u16)actor->posX.parts.integer;
    z = (u16)actor->posZ.parts.integer;
    if (D_8009D1FC->regionCount == 0) {
        return;
    }

    floor_index = 0;
    do {
        if (D_8009D1D8 == 0) {
            floor_table_offset = floor_index << 2;
            floor_table_offset += (u32)D_8009CE08;
            floor_data = *(u8 **)floor_table_offset;
            triangle_index = 0;
            floor_value = ((CollisionFlatRegion *)floor_data)->height;
            triangle_count = ((CollisionFlatRegion *)floor_data)->triangleCount;
            initial_indices = ((CollisionFlatRegion *)floor_data)->triangleIndices;
            indices = initial_indices;
            for (triangle_index = 0; triangle_index < triangle_count;
                 triangle_index++, indices++) {
                    triangle_id = *indices;
                    floor_offset = 11 * triangle_id;
                    simple_triangle = (u8 *)D_8009D1FC->triangles.pointer +
                                      floor_offset * 2;
                    if (Geo_PointInTri(simple_triangle, (s16)x, (s16)z)) {
                        actor->collisionFace = simple_triangle;
                        actor->collisionFaceMirror = simple_triangle;
                        if ((actor->entityFlags & 2) == 0) {
                            actor->posY.fixed = floor_value << 16;
                        }
                        return;
                    }
            }
        } else {
            floor_table_offset = floor_index << 2;
            floor_table_offset += (u32)D_8009CE08;
            floor_data = *(u8 **)floor_table_offset;
            triangle_index = 0;
            triangle_count = ((CollisionPlaneRegion *)floor_data)->triangleCount;
            initial_indices = ((CollisionPlaneRegion *)floor_data)->triangleIndices;
            indices = initial_indices;
            for (triangle_index = 0; triangle_index < triangle_count;
                 triangle_index++, indices++) {
                    triangle_id = *indices;
                    floor_offset = triangle_id * 28;
                    complex_triangle = (CollisionFace *)((u8 *)D_8009D1FC->triangles.pointer + floor_offset);
                    if (Geo_PointInTri(complex_triangle, (s16)x, (s16)z)) {
                        actor->collisionFace = complex_triangle;
                        actor->collisionFaceMirror = complex_triangle;
                        if ((actor->entityFlags & 2) == 0) {
                            x_height = Math_FixedMul(
                                D_8009D1D8[complex_triangle->plane].a,
                                actor->posX.fixed);
                            z_height = Math_FixedMul(
                                D_8009D1D8[complex_triangle->plane].c,
                                actor->posZ.fixed);
                            actor->posY.fixed = Math_FixedMul(
                                complex_triangle->distance - x_height - z_height,
                                D_8009D1D8[complex_triangle->plane].inverseB);
                        }
                        return;
                    }
            }
        }
        floor_index++;
    } while (floor_index < D_8009D1FC->regionCount);
}



void Entity_ResolvePosition(BattleEntity *actor, int index) {
    BattleEntity *actor_s2;
    CollisionPlane *table_base;
    register int index_v1 asm("$3");
    int arg_y;
    int arg_z;
    CollisionTriangleHeader *entry;
    int value;

    actor_s2 = actor;
    table_base = g_CollisionPlaneTable;
    index_v1 = index;

    if (table_base == 0) {
        char *base;
        CollisionFlatRegion **table;
        int idx;

        idx = index_v1 & 0xFFFF;
        base = (char *)g_CollisionDb;
        base = (char *)((CollisionDatabase *)base)->triangles.pointer;
        entry = (CollisionTriangleHeader *)&((CollisionTriangleXZ *)base)[idx];
        actor_s2->collisionFace = entry;
        actor_s2->collisionFaceMirror = entry;
        table = (CollisionFlatRegion **)g_RegionHeightTable;
        value = table[entry->region]->height;
        arg_y = actor_s2->posX.parts.integer;
        arg_z = actor_s2->posZ.parts.integer;
        value <<= 16;
        actor_s2->posY.fixed = value;
    } else {
        CollisionFace *entry_s0;
        int first;
        int idx;
        int id;
        int second;

        idx = index_v1 & 0xFFFF;
        entry_s0 = &g_CollisionDb->triangles.xyz[idx].face;
        actor_s2->collisionFace = entry_s0;
        actor_s2->collisionFaceMirror = entry_s0;
        {
            int id_v1;
            id_v1 = entry_s0->plane;
            id = id_v1;
        }
        first = Math_FixedMul(table_base[id].a, actor_s2->posX.fixed);
        id = entry_s0->plane;
        second = Math_FixedMul(g_CollisionPlaneTable[id].c, actor_s2->posZ.fixed);
        id = entry_s0->plane;
        value = Math_FixedMul(entry_s0->distance - first - second, g_CollisionPlaneTable[id].inverseB);
        actor_s2->posY.fixed = value;
        arg_y = actor_s2->posX.parts.integer;
        arg_z = actor_s2->posZ.parts.integer;
        entry = (CollisionTriangleHeader *)entry_s0;
    }

    Geo_PointInTri(entry, arg_y, arg_z);
    actor_s2->baseX = actor_s2->posX.fixed;
    actor_s2->baseY = actor_s2->posY.fixed;
    actor_s2->baseZ = actor_s2->posZ.fixed;
}

/* Undoes the move: the actor goes back to its last valid position. */
static inline void Entity_RevertMove(BattleEntity *actor)
{
    actor->posX.fixed = actor->baseX;
    actor->posY.fixed = actor->baseY;
    actor->posZ.fixed = actor->baseZ;
    actor->entityFlags |= 0x80000;
}

/* Keeps an actor on the walkable mesh after it moved: finds the triangle
 * under the new position (sliding the player along a ramp edge first when
 * needed), then snaps the height to the plane or region floor, or rolls the
 * move back when no floor is reachable. */
void Entity_UpdateAndRender(BattleEntity *actor)
{
    s16 x, z, oldX, oldZ;
    void *face;
    CollisionPlane *planes;
    u16 saved, radius;
    unsigned int i, j;

    x = actor->posX.fixed >> 16;
    z = actor->posZ.fixed >> 16;
    actor->entityFlags &= ~0x80000;
    oldX = actor->baseX >> 16;
    oldZ = actor->baseZ >> 16;
    if (oldX == x && oldZ == z && actor->posY.fixed == actor->baseY) {
        if (actor == D_8009D254.actor)
            D_8009D2E8.flags &= ~8;
        return;
    }

    radius = actor->renderObject.hit_cylinder.radius;
    face = actor->collisionFace;
    actor->collisionFaceMirror = face;
    D_8009CE2C = radius;
    D_8009CE2C = radius * actor->moveSpeed / 4096;

    if (actor == D_8009D254.actor) {
        if (D_8009D2E8.flags & 8) {
            int area, oldArea, a, b;
            int sxy;

            /* Signed areas of the ramp edge against the new and the old
             * position. */
            sxy = (z << 16) | (u16)x;
            gte_ldsxy3(D_8009CE0C.packed, D_8009CE10.packed, sxy);
            gte_nclip();
            gte_stmac0(&area);
            sxy = (oldZ << 16) | (u16)oldX;
            gte_ldsxy2(sxy);
            gte_nclip();
            gte_stmac0(&oldArea);
            a = area;
            b = oldArea;
            /* Still on the edge when the actor crossed it or did not move
             * closer to it, and the radius stays inside the edge box. */
            if ((a ^ b) < 0 || (a = abs(a), b >= 0 ? b >= a : -b >= a)) {
                u16 r = D_8009CE2C;
                u16 maxX = D_8009CE1C, minX = D_8009CE20;
                u16 maxZ = D_8009CE24, minZ = D_8009CE28;
                s16 width = maxX - minX;
                s16 height = maxZ - minZ;
                int d;

                d = x - r;
                if (!((s16)maxX < d &&
                      (width > height || z > (s16)maxZ ||
                       (d = z + r) < (s16)minZ))) {
                    d = x + r;
                    if (!(d < (s16)minX &&
                          (width > height || (s16)maxZ < (d = z - r) ||
                           (d = z + r) < (s16)minZ))) {
                        d = z - r;
                        if (!((s16)maxZ < d &&
                              (height > width || (s16)maxX < (d = x - r) ||
                               (d = x + r) < (s16)minX))) {
                            d = z + r;
                            if (!(d < (s16)minZ &&
                                  (height > width || (s16)maxX < (d = x - r) ||
                                   (d = x + r) < (s16)minX)))
                                /* Recorded debt: retail enters the slide
                                 * code from here, past the first floor
                                 * search (goto). */
                                goto slide;
                        }
                    }
                }
            }
        }
        D_8009D2E8.flags &= ~8;
    }

    for (i = 0; i < D_8009D1FC->visitedWordCount; i++)
        D_8009DFB0[i] = 0;
    if (!Geo_ClipToFloorBoundary(x, z, face)) {
        if (actor != D_8009D254.actor) {
            Entity_RevertMove(actor);
            return;
        }
    slide:
        Entity_SlideOnRamp(actor);
        x = actor->posX.parts.integer;
        z = actor->posZ.parts.integer;
        for (j = 0; j < D_8009D1FC->visitedWordCount; j++)
            D_8009DFB0[j] = 0;
        saved = D_8009CE18;
        i = Geo_ClipToFloorBoundary(x, z, face);
        if (!i) {
            if (saved == D_8009CE18) {
                for (j = 0; j < D_8009D1FC->visitedWordCount; j++)
                    D_8009DFB0[j] = 0;
                D_8009DFB0[D_8009CE18 >> 5] = 1 << (D_8009CE18 & 0x1F);
                i = Geo_ClipToFloorBoundary(x, z, face);
                if (i) {
                    Entity_RevertMove(actor);
                    return;
                }
            }
            Entity_SlideOnRamp(actor);
            x = actor->posX.parts.integer;
            z = actor->posZ.parts.integer;
            for (j = 0; j < D_8009D1FC->visitedWordCount; j++)
                D_8009DFB0[j] = 0;
            i = Geo_ClipToFloorBoundary(x, z, face);
            if (!i) {
                Entity_RevertMove(actor);
                return;
            }
        }
    }

    if (!Geo_PointInTri(face, x, z))
        face = Geo_ClipToFloorBoundarySub(face, 0, x, z, oldX, oldZ);

    if ((planes = D_8009D1D8) != 0) {
        CollisionFace *floor = face;

        if (actor->entityFlags & 2) {
            int y = actor->posY.fixed;
            int a = Math_FixedMul(planes[floor->plane].a, actor->posX.fixed);
            int c = Math_FixedMul(D_8009D1D8[floor->plane].c, actor->posZ.fixed);

            actor->posY.fixed = Math_FixedMul(floor->distance - a - c,
                                              D_8009D1D8[floor->plane].inverseB);
            if (y < actor->posY.fixed) {
                actor->posY.fixed = y;
            } else {
                actor->entityFlags &= ~2;
                actor->motionY = 0;
            }
        } else {
            int a = Math_FixedMul(planes[floor->plane].a, actor->posX.fixed);
            int c = Math_FixedMul(D_8009D1D8[floor->plane].c, actor->posZ.fixed);

            actor->posY.fixed = Math_FixedMul(floor->distance - a - c,
                                              D_8009D1D8[floor->plane].inverseB);
        }
    } else {
        CollisionTriangleHeader *floor = face;
        u8 region = floor->region;
        u32 flags = actor->entityFlags;
        int height = ((CollisionFlatRegion *)D_8009CE08[region])->height;

        if (flags & 2) {
            if (actor->posY.fixed >= height && actor->baseY < height) {
                actor->entityFlags = flags & ~2;
                actor->posY.fixed = height;
                actor->motionY = 0;
            }
        } else if (region != ((CollisionTriangleHeader *)actor->collisionFace)->region) {
            int delta;
            int step;

            delta = height - actor->posY.parts.integer;
            step = actor->stepHeight;
            if (delta < 0)
                delta = -delta;
            step <<= 16;
            if (delta >= step) {
                Entity_RevertMove(actor);
                return;
            }
            actor->posY.fixed = height << 16;
        }
    }
    actor->collisionFace = face;
}

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
        CollisionTriangleXZ *tri = triangle;
        int px = x;
        int pz = z;
        u32 slot = 0;
        CollisionVertexTable vertex;

        nextIndex = tri->layout.links.vertices[2];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXZ);
        nextX = vertex.xz->x;
        nextZ = vertex.xz->z;
        do {
            u32 bit = tri->layout.links.edgeIds[slot];

            prevIndex = nextIndex;
            prevX = nextX;
            prevZ = nextZ;
            bits = &D_8009DFB0[bit >> 5];
            bit = 1 << (bit & 0x1F);
            nextIndex = tri->layout.links.vertices[slot];
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
                edgeId = tri->layout.links.edgeIds[slot];
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
                    u16 neighbour = tri->layout.links.neighbours[slot];

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
        CollisionTriangleXYZ *tri = triangle;
        int px = x;
        int pz = z;
        u32 slot = 0;
        CollisionVertexTable vertex;

        nextIndex = tri->layout.links.vertices[2];
        COLLISION_VERTEX(vertex, nextIndex, CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
        do {
            u32 bit = tri->layout.links.edgeIds[slot];

            prevIndex = nextIndex;
            prevX = nextX;
            prevZ = nextZ;
            bits = &D_8009DFB0[bit >> 5];
            bit = 1 << (bit & 0x1F);
            nextIndex = tri->layout.links.vertices[slot];
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
                edgeId = tri->layout.links.edgeIds[slot];
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
                    u16 neighbour = tri->layout.links.neighbours[slot];

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

/* Floor-mesh triangle walk: follow a segment across neighbouring walkable
 * triangles, and the point-in-triangle test it uses. Contiguous -G8 pair on
 * the collision database (main_tu_evidence G0227 joins the whole floor
 * resolution range). */

/* Walks from the triangle `indices` across the edge crossed by the segment
 * (x0, z0)-(x1, z1) into neighbouring triangles, never stepping back into
 * `previous`, until one contains the start point. Returns that triangle or
 * 0 when the segment leaves the walkable mesh. */
void *Geo_ClipToFloorBoundarySub(void *triangle, void *previous, s16 x0, s16 z0,
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

        COLLISION_VERTEX(vertex, ((CollisionTriangleXYZ *)triangle)->layout.links.vertices[2], CollisionVertexXYZ);
        nextX = vertex.xyz->x;
        nextZ = vertex.xyz->z;
    } else {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, ((CollisionTriangleXZ *)triangle)->layout.links.vertices[2], CollisionVertexXZ);
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

            COLLISION_VERTEX(vertex, ((CollisionTriangleXYZ *)triangle)->layout.links.vertices[edge], CollisionVertexXYZ);
            nextX = vertex.xyz->x;
            nextZ = vertex.xyz->z;
        } else {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, ((CollisionTriangleXZ *)triangle)->layout.links.vertices[edge], CollisionVertexXZ);
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
            neighbour = &D_8009D1FC->triangles.xz[((CollisionTriangleXZ *)triangle)->layout.links.neighbours[edge]];
            if (neighbour == previous)
                continue;
            if (Geo_PointInTri(neighbour, x0, z0))
                return neighbour;
            neighbour = Geo_ClipToFloorBoundarySub(neighbour, triangle, x0, z0,
                                                   x1, z1);
            if (neighbour)
                return neighbour;
        } else {
            neighbour = &D_8009D1FC->triangles.xyz[((CollisionTriangleXYZ *)triangle)->layout.links.neighbours[edge]];
            if (neighbour == previous)
                continue;
            if (Geo_PointInTri(neighbour, x0, z0))
                return neighbour;
            neighbour = Geo_ClipToFloorBoundarySub(neighbour, triangle, x0, z0,
                                                   x1, z1);
            if (neighbour)
                return neighbour;
        }
    }
    return 0;
}

int Geo_PointInTri(void *triangle, s16 x, s16 z)
{
    u16 nextX, nextZ, previousX, previousZ;
    unsigned edge;
    unsigned inside;

    if (D_8009D1D8) {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, ((CollisionTriangleXYZ *)triangle)->layout.links.vertices[2], CollisionVertexXYZ);
        nextZ = vertex.xyz->z;
        nextX = vertex.xyz->x;
    } else {
        CollisionVertexTable vertex;

        COLLISION_VERTEX(vertex, ((CollisionTriangleXZ *)triangle)->layout.links.vertices[2], CollisionVertexXZ);
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

            COLLISION_VERTEX(vertex, ((CollisionTriangleXYZ *)triangle)->layout.links.vertices[edge], CollisionVertexXYZ);
            nextZ = vertex.xyz->z;
            nextX = vertex.xyz->x;
        } else {
            CollisionVertexTable vertex;

            COLLISION_VERTEX(vertex, ((CollisionTriangleXZ *)triangle)->layout.links.vertices[edge], CollisionVertexXZ);
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

/* Matching debt: independent views preserve the absolute flag load/store.
 * Both names refer to one word; the array declarations control addressing. */
extern u32 rampFlagsRead[4] asm("D_8009D2E8");
extern u32 rampFlagsWrite[4] asm("D_8009D2E8");

void Entity_SlideOnRamp(BattleEntity *entity)
{
    s32 x = entity->baseX;
    s32 z = entity->baseZ;
    s32 deltaX = entity->posX.fixed - x;
    s32 directionX = D_8009CE14[D_8009CE18].directionX;
    s32 directionZ = D_8009CE14[D_8009CE18].directionZ;
    s32 projection, numerator;
    u32 loopEdgeX, loopEdgeZ;
    s32 edgeX, edgeZ;
    s32 radius, direction;

    rampFlagsWrite[0] = rampFlagsRead[0] | 8;
    projection = Math_FixedMul(directionX, deltaX);
    projection += Math_FixedMul(directionZ, entity->posZ.fixed - z);
    x = Math_FixedMul(directionX, projection);
    z = Math_FixedMul(directionZ, projection);
    z += entity->baseZ;
    x += entity->baseX;

    /* Retail keeps the low word of the cross product before signed division. */
    edgeX = D_8009CE10.point.x - D_8009CE0C.point.x;
    edgeZ = D_8009CE10.point.z - D_8009CE0C.point.z;
    numerator = ((z >> 16) - D_8009CE0C.point.z) * edgeX
                - ((x >> 16) - D_8009CE0C.point.x) * edgeZ;
    projection = D_8009CE14[D_8009CE18].length.parts.integer;
    projection = numerator / projection;
    if (projection < 0)
        projection = -projection;
    if (projection > D_8009CE2C) {
        entity->posX.fixed = x;
        entity->posZ.fixed = z;
        return;
    }

    /* Search outward in +X, -X, +Z, -Z order until clear of the edge. */
    for (radius = 0; ; radius++) {
        for (direction = 0; direction < 4; ++direction) {
            directionX = x;
            directionZ = z;
            switch (direction) {
            case 0: directionX += radius << 16; break;
            case 1: directionX -= radius << 16; break;
            case 2: directionZ += radius << 16; break;
            case 3: directionZ -= radius << 16; break;
            }
            loopEdgeX = D_8009CE10.point.x - D_8009CE0C.point.x;
            loopEdgeZ = D_8009CE10.point.z - D_8009CE0C.point.z;
            numerator = ((directionZ >> 16) - D_8009CE0C.point.z) * loopEdgeX
                        - ((directionX >> 16) - D_8009CE0C.point.x) * loopEdgeZ;
            projection = D_8009CE14[D_8009CE18].length.parts.integer;
            projection = numerator / projection;
            if (projection < 0)
                projection = -projection;
            if (projection > D_8009CE2C) {
                entity->posX.fixed = directionX;
                entity->posZ.fixed = directionZ;
                return;
            }
        }
    }
}

/* Containment in the shared 16.16 floor polygon vertices. */

int Geo_PointInPoly(int x, int z, const PolygonVertex *vertices, unsigned short count)
{
    unsigned short i;
    /* Matching debt: the parity accumulator remains pinned on stock GCC
     * 2.7.2. The edge comparison result is an ordinary flag. */
    register int inside asm("$11");
    int previousX, previousZ, currentX, currentZ;
    int cross, dz;
    short intersects;
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
