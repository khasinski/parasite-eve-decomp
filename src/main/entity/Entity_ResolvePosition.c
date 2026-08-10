#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern char *g_CollisionDb;
extern char *g_CollisionPlaneTable;
extern char *g_RegionHeightTable;

int Math_FixedMul(int arg0, int arg1);
int Geo_PointInTri(char *arg0, int arg1, int arg2);

void Entity_ResolvePosition(BattleEntity *actor, int index) {
    BattleEntity *actor_s2;
    char *table_base;
    register int index_v1 asm("$3");
    int arg_y;
    int arg_z;
    char *entry;
    int value;

    actor_s2 = actor;
    table_base = g_CollisionPlaneTable;
    index_v1 = index;

    if (table_base == 0) {
        char *base;
        char *table;
        int idx;

        idx = index_v1 & 0xFFFF;
        base = g_CollisionDb;
        base = *(char **)(base + 0x1C);
        entry = base + (idx * 22);
        actor_s2->collisionFace = entry;
        actor_s2->collisionFaceMirror = entry;
        table = g_RegionHeightTable;
        value = *(short *)(*(char **)(table + ((u8)entry[1] * 4)));
        arg_y = actor_s2->posX.parts.integer;
        arg_z = actor_s2->posZ.parts.integer;
        value <<= 16;
    } else {
        char *entry_s0;
        int first;
        char *base;
        int offset;
        int idx;
        int id;
        int second;

        idx = index_v1 & 0xFFFF;
        offset = idx << 3;
        base = g_CollisionDb;
        offset -= idx;
        entry_s0 = *(char **)(base + 0x1C);
        offset <<= 2;
        entry_s0 += offset;
        actor_s2->collisionFace = entry_s0;
        actor_s2->collisionFaceMirror = entry_s0;
        {
            int id_v1;
            id_v1 = *(u16 *)(entry_s0 + 2);
            id = id_v1;
        }
        first = Math_FixedMul(*(int *)(table_base + (id * 12) + 0), actor_s2->posX.fixed);
        id = *(u16 *)(entry_s0 + 2);
        second = Math_FixedMul(*(int *)(g_CollisionPlaneTable + (id * 12) + 8), actor_s2->posZ.fixed);
        id = *(u16 *)(entry_s0 + 2);
        value = Math_FixedMul(*(int *)(entry_s0 + 4) - first - second, *(int *)(g_CollisionPlaneTable + (id * 12) + 4));
        arg_y = actor_s2->posX.parts.integer;
        arg_z = actor_s2->posZ.parts.integer;
        asm volatile("" : : "r"(arg_y), "r"(arg_z));
        entry = entry_s0;
    }

    actor_s2->posY.fixed = value;
    Geo_PointInTri(entry, arg_y, arg_z);
    actor_s2->baseX = actor_s2->posX.fixed;
    actor_s2->baseY = actor_s2->posY.fixed;
    actor_s2->baseZ = actor_s2->posZ.fixed;
}
