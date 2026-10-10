#ifndef PE1_FIELD_COLLISION_H
#define PE1_FIELD_COLLISION_H

#include "common.h"

/* Little-endian 16.16 X/Z pair. The containment test reads only the
 * signed integer halves, at offsets 2 and 6 of each eight-byte vertex. */
typedef struct {
    unsigned short xFraction;
    short x;
    unsigned short zFraction;
    short z;
} PolygonVertex;

/* vertices must contain count > 0 entries. x and z are 16.16 coordinates. */
int Geo_PointInPoly(int x, int z, const PolygonVertex *vertices,
                    unsigned short count);

struct BattleEntity;

/* 16.16 edge length; the slide reads only the integer half. */
typedef union RampEdgeLength {
    s32 fixed;
    struct {
        u16 fraction;
        s16 integer;
    } parts;
} RampEdgeLength;

/* Floor edge record (D_8009CE14, one per edge id): 16.16 length and unit
 * direction in the X/Z plane. */
typedef struct RampEdge {
    RampEdgeLength length;
    s32 directionX;
    s32 directionZ;
} RampEdge;

/* Endpoints of the edge the player last crossed onto a ramp, as screen
 * style XY pairs: the GTE normal clip reads each point as one packed word. */
typedef union FloorEdgePoint {
    struct {
        s16 x, z;
    } point;
    u32 packed;
} FloorEdgePoint;
extern FloorEdgePoint D_8009CE0C, D_8009CE10;
extern RampEdge *D_8009CE14;
extern u16 D_8009CE18;
void Entity_SlideOnRamp(struct BattleEntity *entity);

PE1_STATIC_ASSERT(sizeof(RampEdge) == 12, ramp_edge_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RampEdge, length.parts.integer) == 2, ramp_edge_length_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RampEdge, directionX) == 4, ramp_edge_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RampEdge, directionZ) == 8, ramp_edge_z_offset);

/* Shared collision query state. The response routine publishes the scaled
 * half-width before querying and sliding against this polygon. */
extern unsigned short D_8009CE2C;
extern PolygonVertex *D_8009D2F8;
extern unsigned short D_8009D264;

int Geo_FindNearestEdge(int x, int z, PolygonVertex *vertices,
                        unsigned short count);
void Entity_SlideOnWall(struct BattleEntity *actor, const PolygonVertex *vertices,
                       unsigned short count, short edge, int oldX, int oldZ);
/* The retail caller passes an argument; this routine uses D_8009D254. */
struct FieldActor;
void Entity_ApplyCollisionResponse(struct FieldActor *player);

#include "pe1/collision_database.h"

/* Vertex `index` of the active table, addressed as retail does. */
#define COLLISION_VERTEX(address, index, type) \
    ((address).word = (index) * sizeof(type) + D_8009D1FC->vertices.word)

extern CollisionDatabase *D_8009D1FC;

/* Sloped-mode plane A*x + B*y + C*z = D, with B stored inverted. */
typedef struct CollisionPlane {
    s32 a;
    s32 inverseB;
    s32 c;
} CollisionPlane;

/* Plane-table pointer; existing floor queries require a fresh read. */
extern CollisionPlane *volatile D_8009D1D8;
/* Flat mode: per-region records that start with the region height. */
extern s16 **D_8009CE08;
/* Bounding box of the ramp edge the player last crossed. */
extern u16 D_8009CE1C, D_8009CE20, D_8009CE24, D_8009CE28;
/* One bit per triangle already visited by the floor search. */
extern u32 D_8009DFB0[];
int Geo_PointInTri(void *triangle, s16 x, s16 z);
void *Geo_ClipToFloorBoundarySub(void *triangle, void *previous, s16 x0, s16 z0,
                                 s16 x1, s16 z1);

PE1_STATIC_ASSERT(sizeof(CollisionVertexXZ) == 4, collision_vertex_xz_size);
PE1_STATIC_ASSERT(sizeof(CollisionVertexXYZ) == 6, collision_vertex_xyz_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(CollisionDatabase, vertices) == 0x18,
                  collision_database_vertices_offset);
PE1_STATIC_ASSERT(sizeof(CollisionTriangleXZ) == 22, collision_triangle_xz_size);
PE1_STATIC_ASSERT(sizeof(CollisionTriangleXYZ) == 28, collision_triangle_xyz_size);

/* Field floor / collision geometry (what keeps Aya on the walkable mesh and
 * sets her ground height Y). See field_movement.h for how motion drives pos;
 * after integration, Entity_ResolvePosition() snaps pos_y to the floor and the
 * current triangle is found by point-in-polygon test.
 *
 * Verified live via DuckStation GDB on the first field map (flat/region mode).
 *
 * The walkable area is a 2D mesh (in the X/Z plane) of triangles/regions:
 *   D_8009D1FC  collision DB base [live: 0x801aa85c]
 *     +0x18  vertex table  (vertices: u16 x@+0, u16 z; stride 6 plane-mode / 4 flat-mode)
 *     +0x1C  triangle/region entry table (stride 28 plane-mode / 22 flat-mode)
 *   D_8009D1D8  plane-equation table, OR 0 -> selects the height mode (see below)
 *   D_8009CE08  region -> height table (flat mode only)
 *
 * Entry (one per walkable triangle/region):
 *   +0x01  u8   region/height id        (flat mode: index into D_8009CE08)
 *   +0x02  u16  plane id                (plane mode: index into D_8009D1D8)
 *   +0x04  s32  plane D term            (plane mode)
 *   +0x02/+0x06/+0x08/+0x0C  vertex/neighbor indices walked by Geo_PointInTri
 *
 * TWO HEIGHT MODES (chosen by D_8009D1D8):
 *   1) FLAT / region mode  (D_8009D1D8 == 0)  [this map]:
 *        Y = D_8009CE08[entry[1]] << 16     (per-region constant height; here Y=0)
 *   2) SLOPED / plane mode (D_8009D1D8 != 0):
 *        plane[id] = D_8009D1D8 + id*12 : { s32 A@+0, s32 invB@+4, s32 C@+8 }
 *        Y = FixedMul( entryD - FixedMul(A, pos_x) - FixedMul(C, pos_z), invB )
 *        i.e. solve the triangle's plane equation A*x + B*y + C*z = D for y.
 *   The resolved Y is written to pos_y (FieldActor +0x2C), then base (+0x40..) is
 *   refreshed for rollback.
 *
 * The current triangle is cached on the actor:
 *   FieldActor +0x1A4 / +0x1A8 : pointer(s) to the current entry.
 *
 * Geo_PointInTri(entry, x, z): point-in-polygon edge-walk over the entry's
 * vertices (looked up in the +0x18 vertex table). Uses the INTEGER world coords
 * pos_x>>16 / pos_z>>16 (FieldActor reads them as s16 at +0x2A / +0x32). Drives
 * which region the actor is in and rejects positions outside the walkable mesh.
 *
 * COLLISION RESPONSE (wall blocking) -- Entity_RollbackPositionHierarchy:
 *   When the post-move position is off the walkable mesh, the actor is REVERTED
 *   to the position saved before the move:
 *     pos(+0x28/+0x2C/+0x30) <- base(+0x40/+0x44/+0x48)   (saved by Entity_IntegratePositionFull)
 *     current-tri(+0x1A4)    <- prev-tri(+0x1A8)
 *     flags(+0x98) |= 0x40000                              ("blocked this frame")
 *   So walls work by "move, test containment, revert if outside" (no slide in
 *   the flat map). Actor groups link via +0x18C (parent/child); the rollback
 *   recurses over the child and every entity sharing the same +0x18C parent,
 *   so a whole grouped formation reverts together.
 *   The per-frame actor contact pass Scene_UpdateEntityPositions walks every
 *   pair in the entity list D_8009D20C (skipping actors with flags(+0x98)&0x20),
 *   tests body spheres and x/z cylinders, and rolls back actors that move into
 *   each other through Entity_RollbackPositionHierarchy.
 *
 * Functions:
 *   void Entity_ResolvePosition(FieldActor *a, int triIndex);  // snap Y + cache tri
 *   int  Entity_ResolveCurrentPosition(u16 **idx);             // resolve D_8009D2F0
 *   s32  Geo_PointInTri(u8 *entry, s16 x, s16 z);              // containment test
 *   void Entity_RollbackPositionHierarchy(FieldActor *a);      // revert pos<-base on block
 *   void Scene_UpdateEntityPositions(void);                    // per-frame actor contact pass
 *   int  Math_FixedMul(int a, int b);                          // (a*b)>>12 fixed-point
 */

#endif /* PE1_FIELD_COLLISION_H */
