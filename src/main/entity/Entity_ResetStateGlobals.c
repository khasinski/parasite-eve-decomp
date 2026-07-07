/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_RegionHeightTable;
extern int g_SceneDataTable2;
extern short g_FieldCollisionTriIndex;
extern short D_8009CE1C;
extern short D_8009CE20;
extern short D_8009CE24;
extern short D_8009CE28;
extern short g_EntityCollisionMoveDelta;
extern short D_8009D1CC;
extern int g_CollisionPlaneTable;
extern int g_CollisionDb;
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
