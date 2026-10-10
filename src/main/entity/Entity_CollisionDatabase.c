/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/collision_database.h"

/* The room collision database: clearing the per-room collision state and
 * relocating the loaded database's offsets into pointers. */

extern char *D_800B1620[];
extern char *g_SceneDataTable2;
extern short g_FieldCollisionTriIndex;
extern short D_8009CE1C;
extern short D_8009CE20;
extern short D_8009CE24;
extern short D_8009CE28;
extern short g_EntityCollisionMoveDelta;
extern short D_8009D1CC;
extern int D_8009D248;
extern short g_EntityCollisionWallParam;
extern int g_EntityCollisionWallSlot;
extern int D_8009DFB0[];

extern char D_8009CE0C[];
extern char D_8009CE0E[];

void Entity_ResetStateGlobals(void) {
    unsigned int i;
    unsigned int count;
    int *ptr;

    g_RegionHeightTable = 0;

    for (i = 0; i < 8; i += 4) {
        *(short *)&D_8009CE0C[i] = 0;
        *(short *)&D_8009CE0E[i] = 0;
    }

    count = 0;
    g_SceneDataTable2 = 0;
    ptr = D_8009DFB0;

    while (count < 20) {
        *ptr = 0;
        count++;
        ptr++;
    }

    g_FieldCollisionTriIndex = 0;
    D_8009CE28 = 0;
    D_8009CE24 = 0;
    D_8009CE20 = 0;
    D_8009CE1C = 0;
    g_EntityCollisionMoveDelta = 0;
    g_CollisionPlaneTable = 0;
    g_CollisionDb = 0;
    g_EntityCollisionWallSlot = 0;
    D_8009D248 = 0;
    g_EntityCollisionWallParam = 0;
    D_8009D1CC = 0;
}

void Entity_RelocateSceneData(void) {
    CollisionDatabase *base;
    register char *loaded asm("$2");
    char frame[8];
    u32 offset;

    loaded = D_800B1620[0];
    base = (CollisionDatabase *)loaded;
    offset = *(u32 *)&base->vertices;
    g_CollisionDb = base;

    if (0x80000000U < offset) {
        struct CollisionPlane *planes;
        s16 **heights;

        planes = base->planes.pointer;
        heights = (s16 **)base->regions;
        g_RegionHeightTable = heights;
        g_CollisionPlaneTable = planes;
        return;
    }

    base->vertices.word = (u32)base + offset;
    base->triangles.word = (u32)base + base->triangles.word;
    base->rampEdges.word = (u32)base + base->rampEdges.word;
    if (base->planes.word != 0) {
        base->planes.word = (u32)base + base->planes.word;
    }

    {
        CollisionDatabase *actor;
        u32 *dst;
        u32 i;
        u32 *cursor;
        u16 wordCount;
        u16 *regionCountAddress;
        u32 count;
        u32 entry;

        actor = g_CollisionDb;
        i = 0;
        cursor = (u32 *)actor->planes.pointer;
        wordCount = actor->visitedWordCount;
        entry = actor->rampEdges.word;
        dst = (u32 *)actor->regions;
        wordCount >>= 5;
        g_CollisionPlaneTable = (struct CollisionPlane *)cursor;
        regionCountAddress = &actor->regionCount;
        g_RegionHeightTable = (s16 **)dst;
        count = *regionCountAddress;
        wordCount++;
        g_SceneDataTable2 = (char *)entry;
        actor->visitedWordCount = wordCount;

        if (count != 0) {
            cursor = (u32 *)actor;
            do {
                entry = cursor[10];
                cursor++;
                i++;
                entry = entry + (u32)actor;
                *dst = entry;
                dst++;
            } while (i < actor->regionCount);
        }
    }
}
