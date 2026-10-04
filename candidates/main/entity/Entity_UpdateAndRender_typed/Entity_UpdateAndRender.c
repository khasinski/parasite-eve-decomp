/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 */
#include "pe1/entity_floor.h"
#include "pe1/gte.h"

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
        int area, oldArea, a, b;

        if (!(D_8009D2E8.flags & 8))
            goto leave;
        gte_ldsxy3(D_8009CE0C.packed, D_8009CE10.packed,
                   (u16)x | (z << 16));
        gte_nclip();
        gte_stmac0(&area);
        gte_ldsxy2((oldZ << 16) | (u16)oldX);
        gte_nclip();
        gte_stmac0(&oldArea);
        a = area;
        b = oldArea;
        if ((a ^ b) >= 0) {
            if (a < 0)
                a = -a;
            if ((b >= 0 && b < a) || (b < 0 && -b < a))
                goto leave;
        }
        {
            u16 r = D_8009CE2C;
            u16 maxX = D_8009CE1C, minX = D_8009CE20;
            u16 maxZ = D_8009CE24, minZ = D_8009CE28;
            s16 width = maxX - minX;
            s16 height = maxZ - minZ;
            int d;

            d = x - r;
            if ((s16)maxX < d) {
                if (width > height)
                    goto leave;
                if (z > (s16)maxZ)
                    goto leave;
                d = z + r;
                if (d < (s16)minZ)
                    goto leave;
            }
            d = x + r;
            if (d < (s16)minX) {
                if (width > height)
                    goto leave;
                d = z - r;
                if ((s16)maxZ < d)
                    goto leave;
                d = z + r;
                if (d < (s16)minZ)
                    goto leave;
            }
            d = z - r;
            if ((s16)maxZ < d) {
                if (height > width)
                    goto leave;
                d = x - r;
                if ((s16)maxX < d)
                    goto leave;
                d = x + r;
                if (d < (s16)minX)
                    goto leave;
            }
            d = z + r;
            if (d < (s16)minZ) {
                if (height > width)
                    goto leave;
                d = x - r;
                if ((s16)maxX < d)
                    goto leave;
                d = x + r;
                if (d < (s16)minX)
                    goto leave;
            }
        }
        goto slide;
    leave:
        D_8009D2E8.flags &= ~8;
    }

    for (i = 0; i < D_8009D1FC->faceCount; i++)
        D_8009DFB0[i] = 0;
    if (Geo_ClipToFloorBoundary(x, z, face))
        goto found;
    if (actor != D_8009D254.actor)
        goto blocked;
slide:
    Entity_SlideOnRamp(actor);
    x = actor->posX.parts.integer;
    z = actor->posZ.parts.integer;
    for (j = 0; j < D_8009D1FC->faceCount; j++)
        D_8009DFB0[j] = 0;
    saved = D_8009CE18;
    i = Geo_ClipToFloorBoundary(x, z, face);
    if (i)
        goto found;
    if (saved == D_8009CE18) {
        for (j = 0; j < D_8009D1FC->faceCount; j++)
            D_8009DFB0[j] = 0;
        D_8009DFB0[D_8009CE18 >> 5] = 1 << (D_8009CE18 & 0x1F);
        if (Geo_ClipToFloorBoundary(x, z, face))
            goto blocked;
    }
    Entity_SlideOnRamp(actor);
    x = actor->posX.parts.integer;
    z = actor->posZ.parts.integer;
    for (i = 0; i < D_8009D1FC->faceCount; i++)
        D_8009DFB0[i] = 0;
    if (!Geo_ClipToFloorBoundary(x, z, face))
        goto blocked;
found:
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
            int delta;
            int step;

            delta = height - actor->posY.parts.integer;
            step = actor->stepHeight;
            if (delta < 0)
                delta = -delta;
            step <<= 16;
            if (delta >= step) {
            blocked:
                actor->posX.fixed = actor->baseX;
                actor->posY.fixed = actor->baseY;
                actor->posZ.fixed = actor->baseZ;
                actor->entityFlags |= 0x80000;
                return;
            }
            actor->posY.fixed = height << 16;
        }
    }
    actor->collisionFace = face;
}
