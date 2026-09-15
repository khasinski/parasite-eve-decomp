/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"
#include "pe1/field_collision.h"

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
