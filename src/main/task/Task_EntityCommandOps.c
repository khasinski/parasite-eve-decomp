#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

FieldActor *Scene_LoadMap(char *arg0, FieldActor *arg1, int arg2);
void Entity_FindFloor(FieldActor *arg0);

int Task_SpawnEntityAt(int **arg0) {
    char local[2];
    FieldActor *entity;

    local[0] = arg0[0][0];
    local[1] = arg0[1][0];
    entity = Scene_LoadMap(local, g_CurrentEntity[0], 1);
    entity->pos_x = arg0[2][0];
    entity->pos_y = arg0[3][0];
    entity->pos_z = arg0[4][0];
    Entity_FindFloor(entity);
    return 1;
}
int Task_AssignValue(int **arg0) {
    *arg0[0] = *arg0[1];
    return 1;
}

void Render_SetupColorTable(s16 arg0, int arg1, s16 *arg2);

int Task_StopEntityAnim(s16 **arg0) {
    s16 value[5];

    value[0] = -1;
    Render_SetupColorTable(*arg0[0], 0, value);
    return 1;
}
