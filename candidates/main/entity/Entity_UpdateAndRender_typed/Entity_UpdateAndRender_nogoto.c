/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 */
#include "pe1/entity_floor.h"
#include "pe1/gte.h"

/* No floor reachable: restore the position saved before the move and mark
 * the actor as blocked this frame. */
#define ENTITY_ROLLBACK(actor) \
    { \
        (actor)->posX.fixed = (actor)->baseX; \
        (actor)->posY.fixed = (actor)->baseY; \
        (actor)->posZ.fixed = (actor)->baseZ; \
        (actor)->entityFlags |= 0x80000; \
    }

/* Keeps an actor on the walkable mesh after it moved: finds the triangle
 * under the new position (sliding the player along a ramp edge first when
 * needed), then snaps the height to the plane or region floor, or rolls the
 * move back when no floor is reachable. */
/* Whether the player still stands on the ramp edge it slid along last
 * frame: both positions on the same side of the edge, the new one no
 * farther from it, and the radius box still overlapping the edge box. */
static inline int Entity_StaysOnRampEdge(s16 x, s16 z, s16 oldX, s16 oldZ)
{
    int area, oldArea;
    u16 r, maxX, minX, maxZ, minZ;
    s16 width, height;

    gte_ldsxy3(D_8009CE0C.packed, D_8009CE10.packed, ((u16)z << 16) | (u16)x);
    gte_nclip();
    gte_stmac0(&area);
    gte_ldsxy2(((u16)oldZ << 16) | (u16)oldX);
    gte_nclip();
    gte_stmac0(&oldArea);
    if ((area ^ oldArea) >= 0) {
        if (area < 0)
            area = -area;
        if (oldArea < 0) {
            if (-oldArea < area)
                return 0;
        } else if (oldArea < area) {
            return 0;
        }
    }

    r = D_8009CE2C;
    maxX = D_8009CE1C;
    minX = D_8009CE20;
    maxZ = D_8009CE24;
    minZ = D_8009CE28;
    width = maxX - minX;
    height = maxZ - minZ;
    if ((s16)maxX < x - r) {
        if (height < width || (s16)maxZ < z || z + r < (s16)minZ)
            return 0;
    }
    if (x + r < (s16)minX) {
        if (height < width || (s16)maxZ < z - r || z + r < (s16)minZ)
            return 0;
    }
    if ((s16)maxZ < z - r) {
        if (width < height || (s16)maxX < x - r || x + r < (s16)minX)
            return 0;
    }
    if (z + r < (s16)minZ) {
        if (width < height || (s16)maxX < x - r || x + r < (s16)minX)
            return 0;
    }
    return 1;
}

void Entity_UpdateAndRender(BattleEntity *actor)
{
    s16 x, z, oldX, oldZ;
    void *face;
    u16 saved, radius;
    int found;
    int i;

    x = actor->posX.parts.integer;
    z = actor->posZ.parts.integer;
    actor->entityFlags &= ~0x80000;
    oldX = actor->baseX >> 16;
    oldZ = actor->baseZ >> 16;
    if (oldX == x && oldZ == z && actor->posY.fixed == actor->baseY) {
        if (actor == D_8009D254.actor)
            D_8009D2E8.flags &= ~8;
        return;
    }

    radius = actor->renderObject.table_value70;
    D_8009CE2C = radius;
    face = actor->collisionFace;
    actor->collisionFaceMirror = face;
    D_8009CE2C = radius * actor->moveSpeed / 4096;

    found = 0;
    if (actor != D_8009D254.actor || !(D_8009D2E8.flags & 8)
        || !Entity_StaysOnRampEdge(x, z, oldX, oldZ)) {
        if (actor == D_8009D254.actor)
            D_8009D2E8.flags &= ~8;
        for (i = 0; i < D_8009D1FC->faceCount; i++)
            D_8009DFB0[i] = 0;
        found = Geo_ClipToFloorBoundary(x, z, face);
        if (!found && actor != D_8009D254.actor) {
            ENTITY_ROLLBACK(actor);
            return;
        }
    }
    if (!found) {
        /* Slide along the ramp edge, retrying once with the crossed
         * triangle marked as visited. */
        Entity_SlideOnRamp(actor);
        x = actor->posX.parts.integer;
        z = actor->posZ.parts.integer;
        for (i = 0; i < D_8009D1FC->faceCount; i++)
            D_8009DFB0[i] = 0;
        saved = D_8009CE18;
        if (!Geo_ClipToFloorBoundary(x, z, face)) {
            if (saved == D_8009CE18) {
                for (i = 0; i < D_8009D1FC->faceCount; i++)
                    D_8009DFB0[i] = 0;
                D_8009DFB0[D_8009CE18 >> 5] = 1 << (D_8009CE18 & 0x1F);
                if (Geo_ClipToFloorBoundary(x, z, face)) {
                    ENTITY_ROLLBACK(actor);
                    return;
                }
            }
            Entity_SlideOnRamp(actor);
            x = actor->posX.parts.integer;
            z = actor->posZ.parts.integer;
            for (i = 0; i < D_8009D1FC->faceCount; i++)
                D_8009DFB0[i] = 0;
            if (!Geo_ClipToFloorBoundary(x, z, face)) {
                ENTITY_ROLLBACK(actor);
                return;
            }
        }
    }

    if (!Geo_PointInTri(face, x, z))
        face = Geo_ClipToFloorBoundarySub(face, 0, x, z, oldX, oldZ);

    if (D_8009D1D8) {
        CollisionFace *floor = face;

        if (actor->entityFlags & 2) {
            int y = actor->posY.fixed;
            int a = Math_FixedMul(D_8009D1D8[floor->plane].a, actor->posX.fixed);
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
            int a = Math_FixedMul(D_8009D1D8[floor->plane].a, actor->posX.fixed);
            int c = Math_FixedMul(D_8009D1D8[floor->plane].c, actor->posZ.fixed);

            actor->posY.fixed = Math_FixedMul(floor->distance - a - c,
                                              D_8009D1D8[floor->plane].inverseB);
        }
    } else {
        CollisionFace *floor = face;
        u8 region = floor->region;
        u32 flags = actor->entityFlags;
        int height = D_8009CE08[region][0];

        if (flags & 2) {
            if (actor->posY.fixed >= height && actor->baseY < height) {
                actor->entityFlags = flags & ~2;
                actor->posY.fixed = height;
                actor->motionY = 0;
            }
        } else if (region != ((CollisionFace *)actor->collisionFace)->region) {
            int delta = height - actor->posY.parts.integer;

            if (delta < 0)
                delta = -delta;
            if (delta >= actor->stepHeight << 16) {
                ENTITY_ROLLBACK(actor);
                return;
            }
            actor->posY.fixed = height << 16;
        }
    }
    actor->collisionFace = face;
}
