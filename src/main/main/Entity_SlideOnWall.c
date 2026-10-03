/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/field_movement.h"
#include "pe1/field_collision.h"

/* A vertex pointer seen as the address retail adds the scaled edge to. */
typedef union PolygonVertexAddress {
    const PolygonVertex *vertex;
    u32 word;
} PolygonVertexAddress;

/* Two pins and two empty barriers preserve the retail prologue with stock
 * GCC/maspsx. The first barrier clobbers s3 before the entity is assigned;
 * it emits no instructions. */
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
    PolygonVertexAddress base, point;
    asm("" : "=r"(initial) : "0"(initial) : "$19");
    base.vertex = vertices;
    point.word = edge * sizeof(PolygonVertex) + base.word;
    asm volatile("" : "=r"(initial) : "0"(initial), "r"(point.vertex) : "memory");
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
    unit.x <<= 4;
    unit.z <<= 4;
    x = entity->baseX;
    z = entity->baseZ;
    projection = Math_FixedMul(unit.x, entity->posX.fixed - x);
    projection += Math_FixedMul(unit.z, entity->posZ.fixed - z);
    x = Math_FixedMul(unit.x, projection);
    z = Math_FixedMul(unit.z, projection);
    x += entity->baseX;
    z += entity->baseZ;
    vector.x = startX - endX;
    vector.y = 0;
    vector.z = startZ - endZ;
    length = Gte_ISqrt(vector.x * vector.x + vector.z * vector.z);
    projection = (((z >> 16) - endZ) * vector.x - ((x >> 16) - endX) * vector.z) / length;
    if (projection < 0)
        projection = -projection;
    if (projection > D_8009CE2C) {
        entity->posX.fixed = x;
        entity->posZ.fixed = z;
        return;
    }
    for (radius = 0; ; radius++) {
        for (direction = 0; direction < 4; ++direction) {
            trialX = x;
            trialZ = z;
            switch (direction) {
            case 0: trialX += radius << 16; break;
            case 1: trialX -= radius << 16; break;
            case 2: trialZ += radius << 16; break;
            case 3: trialZ -= radius << 16; break;
            }
            projection = (((trialZ >> 16) - endZ) * vector.x -
                          ((trialX >> 16) - endX) * vector.z) / length;
            if (projection < 0)
                projection = -projection;
            if (projection > D_8009CE2C) {
                entity->posX.fixed = trialX;
                entity->posZ.fixed = trialZ;
                return;
            }
        }
    }
}
