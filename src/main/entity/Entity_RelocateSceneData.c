/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned short u16;
typedef unsigned int u32;

extern char *D_800B1620[];
extern char *g_CollisionDb;
extern char *g_RegionHeightTable;
extern char *g_CollisionPlaneTable;
extern char *g_SceneDataTable2;

void Entity_RelocateSceneData(void) {
    char *base;
    register char *loaded asm("$2");
    char frame[8];
    u32 offset;

    loaded = D_800B1620[0];
    base = loaded;
    offset = *(u32 *)(base + 0x18);
    g_CollisionDb = base;

    if (!(0x80000000U < offset)) {
        goto relocate;
    }

    {
        char *value20;
        char *table;

        value20 = *(char **)(base + 0x20);
        table = base + 0x28;
        g_RegionHeightTable = table;
        g_CollisionPlaneTable = value20;
    }
    return;

relocate:
    *(u32 *)(base + 0x18) = (u32)base + offset;
    *(u32 *)(base + 0x1C) = (u32)base + *(u32 *)(base + 0x1C);
    *(u32 *)(base + 0x24) = (u32)base + *(u32 *)(base + 0x24);
    if (*(u32 *)(base + 0x20) != 0) {
        *(u32 *)(base + 0x20) = (u32)base + *(u32 *)(base + 0x20);
    }

    {
        char *actor;
        u32 *dst;
        u32 i;
        u32 *src;
        register char *value20 asm("$3");
        register char *value24 asm("$4");
        u32 half;
        register u32 count asm("$3");
        register u32 entry asm("$4");

        actor = g_CollisionDb;
        i = 0;
        value20 = *(char **)(actor + 0x20);
        value24 = *(char **)(actor + 0x24);
        half = *(u16 *)(actor + 8);
        dst = (u32 *)(actor + 0x28);
        g_RegionHeightTable = (char *)dst;
        half >>= 5;
        g_CollisionPlaneTable = value20;
        count = *(u16 *)(actor + 2);
        half++;
        g_SceneDataTable2 = value24;
        *(u16 *)(actor + 8) = half;

        if (count != 0) {
            src = (u32 *)actor;
            do {
                entry = src[10];
                src++;
                i++;
                entry = entry + (u32)actor;
                *dst = entry;
                dst++;
            } while (i < *(u16 *)(actor + 2));
        }
    }
}
