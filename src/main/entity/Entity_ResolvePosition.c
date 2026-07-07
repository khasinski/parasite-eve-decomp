/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned short u16;

extern char *g_CollisionDb;
extern char *g_CollisionPlaneTable;
extern char *g_RegionHeightTable;

int Math_FixedMul(int arg0, int arg1);
int Geo_PointInTri(char *arg0, int arg1, int arg2);

void Entity_ResolvePosition(char *actor, int index) {
    char *actor_s2;
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
        *(char **)(actor_s2 + 0x1A4) = entry;
        *(char **)(actor_s2 + 0x1A8) = entry;
        table = g_RegionHeightTable;
        value = *(short *)(*(char **)(table + ((u8)entry[1] * 4)));
        arg_y = *(short *)(actor_s2 + 0x2A);
        arg_z = *(short *)(actor_s2 + 0x32);
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
        *(char **)(actor_s2 + 0x1A4) = entry_s0;
        *(char **)(actor_s2 + 0x1A8) = entry_s0;
        {
            int id_v1;
            id_v1 = *(u16 *)(entry_s0 + 2);
            id = id_v1;
        }
        first = Math_FixedMul(*(int *)(table_base + (id * 12) + 0), *(int *)(actor_s2 + 0x28));
        id = *(u16 *)(entry_s0 + 2);
        second = Math_FixedMul(*(int *)(g_CollisionPlaneTable + (id * 12) + 8), *(int *)(actor_s2 + 0x30));
        id = *(u16 *)(entry_s0 + 2);
        value = Math_FixedMul(*(int *)(entry_s0 + 4) - first - second, *(int *)(g_CollisionPlaneTable + (id * 12) + 4));
        arg_y = *(short *)(actor_s2 + 0x2A);
        arg_z = *(short *)(actor_s2 + 0x32);
        asm volatile("" : : "r"(arg_y), "r"(arg_z));
        entry = entry_s0;
    }

    *(int *)(actor_s2 + 0x2C) = value;
    Geo_PointInTri(entry, arg_y, arg_z);
    *(int *)(actor_s2 + 0x40) = *(int *)(actor_s2 + 0x28);
    *(int *)(actor_s2 + 0x44) = *(int *)(actor_s2 + 0x2C);
    *(int *)(actor_s2 + 0x48) = *(int *)(actor_s2 + 0x30);
}
