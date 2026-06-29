#ifndef PE1_FIELD_COLLISION_H
#define PE1_FIELD_COLLISION_H

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
 *   The per-frame driver Scene_UpdateEntityPositions (raw asm) walks the entity
 *   list D_8009D20C, and for each actor whose flags(+0x98)&0x20 is set, resolves
 *   its region/triangle and applies the snap-or-rollback.
 *
 * Functions:
 *   void Entity_ResolvePosition(FieldActor *a, int triIndex);  // snap Y + cache tri
 *   int  Entity_ResolveCurrentPosition(u16 **idx);             // resolve D_8009D2F0
 *   s32  Geo_PointInTri(u8 *entry, s16 x, s16 z);              // containment test
 *   void Entity_RollbackPositionHierarchy(FieldActor *a);      // revert pos<-base on block
 *   void Scene_UpdateEntityPositions(void);                    // per-frame resolve/rollback driver
 *   int  Math_FixedMul(int a, int b);                          // (a*b)>>12 fixed-point
 */

#endif /* PE1_FIELD_COLLISION_H */
