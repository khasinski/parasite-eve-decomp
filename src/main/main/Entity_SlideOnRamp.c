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
    projection = D_8009CE14[D_8009CE18].length;
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
            projection = D_8009CE14[D_8009CE18].length;
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
