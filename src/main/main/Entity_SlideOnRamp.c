/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/field_movement.h"
#include "pe1/field_collision.h"

/* Matching debt: independent views preserve the absolute flag load/store.
 * Both names refer to one word; the array declarations control addressing. */
extern u32 rampFlagsRead[4] asm("D_8009D2E8");
extern u32 rampFlagsWrite[4] asm("D_8009D2E8");

void Entity_SlideOnRamp(BattleEntity *entity)
{
    s32 x = entity->baseX;
    s32 z = entity->baseZ;
    s32 deltaX = (u32)entity->posX.fixed - (u32)x;
    s32 directionX = D_8009CE14[D_8009CE18].directionX;
    s32 directionZ = D_8009CE14[D_8009CE18].directionZ;
    s32 projection, numerator;
    u32 loopEdgeX, loopEdgeZ;
    s32 edgeX, edgeZ;
    s32 radius, direction;

    rampFlagsWrite[0] = rampFlagsRead[0] | 8;
    projection = Math_FixedMul(directionX, deltaX);
    projection = (u32)projection + (u32)Math_FixedMul(directionZ,
        (u32)entity->posZ.fixed - (u32)z);
    x = Math_FixedMul(directionX, projection);
    z = Math_FixedMul(directionZ, projection);
    z = (u32)z + (u32)entity->baseZ;
    x = (u32)x + (u32)entity->baseX;

    /* Retail keeps the low word of the cross product before signed division. */
    edgeX = D_8009CE10 - D_8009CE0C;
    edgeZ = D_8009CE12 - D_8009CE0E;
    numerator = (s32)((u32)((z >> 16) - D_8009CE0E) * (u32)edgeX
                   - (u32)((x >> 16) - D_8009CE0C) * (u32)edgeZ);
    projection = D_8009CE14[D_8009CE18].length;
    projection = numerator / projection;
    if (projection < 0)
        projection = 0u - (u32)projection;
    if (projection > D_8009CE2C) {
        entity->posX.fixed = x;
        entity->posZ.fixed = z;
        return;
    }

    /* Search outward in +X, -X, +Z, -Z order until clear of the edge. */
    for (radius = 0; ; radius = (u32)radius + 1) {
        for (direction = 0; direction < 4; ++direction) {
            directionX = x;
            directionZ = z;
            switch (direction) {
            case 0: directionX = (u32)directionX + ((u32)radius << 16); break;
            case 1: directionX = (u32)directionX - ((u32)radius << 16); break;
            case 2: directionZ = (u32)directionZ + ((u32)radius << 16); break;
            case 3: directionZ = (u32)directionZ - ((u32)radius << 16); break;
            }
            loopEdgeX = D_8009CE10 - D_8009CE0C;
            loopEdgeZ = D_8009CE12 - D_8009CE0E;
            numerator = (s32)((u32)((directionZ >> 16) - D_8009CE0E) * loopEdgeX
                           - (u32)((directionX >> 16) - D_8009CE0C) * loopEdgeZ);
            projection = D_8009CE14[D_8009CE18].length;
            projection = numerator / projection;
            if (projection < 0)
                projection = 0u - (u32)projection;
            if (projection > D_8009CE2C) {
                entity->posX.fixed = directionX;
                entity->posZ.fixed = directionZ;
                return;
            }
        }
    }
}
