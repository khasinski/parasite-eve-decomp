/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/field_collision.h"
#include "pe1/field_movement.h"
#include "pe1/gte_types.h"

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

/* Returns the first polygon edge (index of its end vertex) whose segment
 * passes within D_8009CE2C of (x, z), or -1 when no edge is that close. */
int Geo_FindNearestEdge(int x, int z, PolygonVertex *vertices,
                        unsigned short count)
{
    GteVector edge;
    s16 pointX = x;
    s16 pointZ = z;
    s16 prevX = vertices[count - 1].x;
    s16 prevZ = vertices[count - 1].z;
    unsigned short i;

    for (i = 0; i < count; i++) {
        s16 startX, startZ;
        int radius, vertexX, vertexZ;
        int endX, endZ;
        int length, distance, offsetX, offsetZ;

        startX = prevX;
        startZ = prevZ;
        radius = D_8009CE2C;
        vertexX = vertices[i].x;
        prevX = vertexX;
        endX = prevX;
        vertexZ = vertices[i].z;
        prevZ = vertexZ;
        distance = pointX - radius;
        if (endX < distance && startX < distance)
            continue;
        distance = pointX + radius;
        if (distance < endX && distance < startX)
            continue;
        endZ = prevZ;
        distance = pointZ - radius;
        if (endZ < distance && startZ < distance)
            continue;
        distance = pointZ + radius;
        if (distance < endZ && distance < startZ)
            continue;
        edge.x = startX - endX;
        edge.y = 0;
        edge.z = startZ - endZ;
        length = Gte_ISqrt(edge.x * edge.x + edge.z * edge.z);
        offsetZ = pointZ - endZ;
        offsetX = pointX - endX;
        distance = (offsetZ * edge.x - offsetX * edge.z) / length;
        if (distance < 0)
            distance = -distance;
        if (distance > D_8009CE2C)
            continue;
        Gte_NormalizeVec(&edge, &edge);
        distance = Math_FixedMul(edge.x << 4, offsetX << 16);
        distance += Math_FixedMul(edge.z << 4, offsetZ << 16);
        if (distance < 0) {
            distance = offsetX * offsetX + offsetZ * offsetZ;
            if (distance > D_8009CE2C * D_8009CE2C)
                continue;
        }
        if (length < distance >> 16) {
            distance = (pointX - startX) * (pointX - startX) +
                       (pointZ - startZ) * (pointZ - startZ);
            if (distance > D_8009CE2C * D_8009CE2C)
                continue;
        }
        return i;
    }
    return -1;
}

/* Retail data at 0x8009D254; the g_PlayerEntity linker alias names this slot
 * too. Explicit .data preserves the non-GP accesses amid small-data globals. */
BattleEntity *D_8009D254 __attribute__((section(".data"))) = 0;

void Entity_ApplyCollisionResponse(int unused)
{
    BattleEntity *actor = D_8009D254;
    int oldX, oldZ;
    short edge;
    int radius = actor->renderObject.table_value70;

    D_8009CE2C = radius;
    /* Preserve the low-word multiply and signed division used by retail. */
    D_8009CE2C = (int)((long long)radius * actor->moveSpeed) / 4096;
    oldX = (short)(actor->baseX >> 16);
    oldZ = (short)(actor->baseZ >> 16);
    edge = Geo_FindNearestEdge(actor->posX.parts.integer, actor->posZ.parts.integer,
                              D_8009D2F8, D_8009D264);
    if (edge >= 0) {
        BattleEntity *current;

        Entity_SlideOnWall(D_8009D254, D_8009D2F8, D_8009D264, edge, oldX, oldZ);
        current = D_8009D254;
        if ((short)Geo_FindNearestEdge(current->posX.parts.integer,
                                      current->posZ.parts.integer,
                                      D_8009D2F8, D_8009D264) >= 0) {
            BattleEntity *restore = D_8009D254;
            restore->posX.fixed = restore->baseX;
            restore->posY.fixed = restore->baseY;
            restore->posZ.fixed = restore->baseZ;
        }
    }
}
