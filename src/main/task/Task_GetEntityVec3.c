#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Task_GetEntityVec3(int **args) {
    switch (*args[0]) {
    case 0:
        *args[1] = g_CurrentEntity->pos_x;
        *args[2] = g_CurrentEntity->pos_y;
        *args[3] = g_CurrentEntity->pos_z;
        break;
    case 1:
        *args[1] = g_CurrentEntity->base_x;
        *args[2] = g_CurrentEntity->base_y;
        *args[3] = g_CurrentEntity->base_z;
        break;
    case 2:
        *args[1] = g_CurrentEntity->motion_x;
        *args[2] = g_CurrentEntity->motion_y;
        *args[3] = g_CurrentEntity->motion_z;
        break;
    case 3:
        *args[1] = g_CurrentEntity->accel_x;
        *args[2] = g_CurrentEntity->accel_y;
        *args[3] = g_CurrentEntity->accel_z;
        break;
    case 4:
        *args[1] = g_CurrentEntity->gravity_x;
        *args[2] = g_CurrentEntity->gravity_y;
        *args[3] = g_CurrentEntity->gravity_z;
        break;
    case 5:
        *args[1] = g_CurrentEntity->rot_x;
        *args[2] = g_CurrentEntity->rot_y;
        *args[3] = g_CurrentEntity->rot_z;
        break;
    case 6:
        *args[1] = g_CurrentEntity->delta_x;
        *args[2] = g_CurrentEntity->delta_y;
        *args[3] = g_CurrentEntity->delta_z;
        break;
    }
    return 1;
}
