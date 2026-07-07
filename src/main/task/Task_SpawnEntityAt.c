/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef struct FieldEntity {
    char pad00[0x28];
    int field28;
    int field2C;
    int field30;
} FieldEntity;

extern FieldEntity *g_CurrentEntity[];

FieldEntity *Scene_LoadMap(char *arg0, FieldEntity *arg1, int arg2);
void Entity_FindFloor(FieldEntity *arg0);

int Task_SpawnEntityAt(int **arg0) {
    char local[2];
    FieldEntity *entity;

    local[0] = arg0[0][0];
    local[1] = arg0[1][0];
    entity = Scene_LoadMap(local, g_CurrentEntity[0], 1);
    entity->field28 = arg0[2][0];
    entity->field2C = arg0[3][0];
    entity->field30 = arg0[4][0];
    Entity_FindFloor(entity);
    return 1;
}
