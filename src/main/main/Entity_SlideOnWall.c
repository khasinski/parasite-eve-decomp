/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/field_movement.h"
#include "pe1/field_collision.h"

/* Two pins and two empty barriers preserve the retail prologue with stock
 * GCC/maspsx. The first barrier clobbers s3 before the entity is assigned;
 * it emits no instructions. Keep the late vector.z read for retail scheduling. */
void Entity_SlideOnWall(BattleEntity *input, const PolygonVertex *vertices,
                       u16 count, s16 edge, int oldX, int oldZ)
{
    register BattleEntity *initial asm("$4") = input;
    register BattleEntity *entity asm("$19");
    GteVector vector, unit;
    s32 endX, endZ;
    s32 startX, startZ;
    s32 x, z, projection, length;
    s32 radius, direction, trialX, trialZ;
    const PolygonVertex *point;
    asm("" : "=r"(initial) : "0"(initial) : "$19");
    point = (const PolygonVertex *)((u32)(edge * sizeof(PolygonVertex)) + (u32)vertices);
    asm volatile("" : "=r"(initial) : "0"(initial), "r"(point) : "memory");
    entity = initial;
    endX = vertices[edge].x;
    endZ = vertices[edge].z;
    if (edge > 0) {
        startX = vertices[edge - 1].x;
        startZ = vertices[edge - 1].z;
    } else {
        startX = vertices[count - 1].x;
        startZ = vertices[count - 1].z;
    }
    vector.x = endX - startX;
    vector.y = 0;
    vector.z = endZ - startZ;
    Gte_NormalizeVec(&vector, &unit);
    unit.x = (u32)unit.x << 4;
    unit.z = (u32)unit.z << 4;
    x = entity->baseX;
    z = entity->baseZ;
    projection = Math_FixedMul(unit.x, (u32)entity->posX.fixed - (u32)x);
    projection = (u32)projection + (u32)Math_FixedMul(unit.z,
        (u32)entity->posZ.fixed - (u32)z);
    x = Math_FixedMul(unit.x, projection);
    z = Math_FixedMul(unit.z, projection);
    x = (u32)x + (u32)entity->baseX;
    z = (u32)z + (u32)entity->baseZ;
    vector.x = startX - endX;
    vector.y = 0;
    vector.z = startZ - endZ;
    length = Gte_ISqrt((u32)vector.x * (u32)vector.x +
                      (u32)vector.z * (u32)vector.z);
    projection = (s32)((u32)((z >> 16) - endZ) * (u32)vector.x -
                    (u32)((x >> 16) - endX) * (u32)vector.z) / length;
    if (projection < 0)
        projection = 0u - (u32)projection;
    if (projection > D_8009CE2C) {
        entity->posX.fixed = x;
        entity->posZ.fixed = z;
        return;
    }
    for (radius = 0; ; radius = (u32)radius + 1) {
        for (direction = 0; direction < 4; ++direction) {
            trialX = x;
            trialZ = z;
            switch (direction) {
            case 0: trialX = (u32)trialX + ((u32)radius << 16); break;
            case 1: trialX = (u32)trialX - ((u32)radius << 16); break;
            case 2: trialZ = (u32)trialZ + ((u32)radius << 16); break;
            case 3: trialZ = (u32)trialZ - ((u32)radius << 16); break;
            }
            projection = (s32)((u32)((trialZ >> 16) - endZ) * (u32)vector.x -
                            (u32)((trialX >> 16) - endX) * (u32)*(volatile s32 *)&vector.z) / length;
            if (projection < 0)
                projection = 0u - (u32)projection;
            if (projection > D_8009CE2C) {
                entity->posX.fixed = trialX;
                entity->posZ.fixed = trialZ;
                return;
            }
        }
    }
}
