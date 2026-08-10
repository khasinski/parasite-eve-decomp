#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;
extern FieldActor *g_PlayerEntity;
extern int g_RenderStateFlags[];

void Entity_FindFloor(void);

int Task_SetEntityVec3(int **args) {
    int **argp = args;

    switch (*argp[0]) {
    case 0:
        {
            register FieldActor *state asm("$4") = g_CurrentEntity;
            state->pos_x = *argp[1];
            state->pos_y = *argp[2];
            state->pos_z = *argp[3];
        }
        Entity_FindFloor();
        g_CurrentEntity->base_x = g_CurrentEntity->pos_x;
        g_CurrentEntity->base_y = g_CurrentEntity->pos_y;
        g_CurrentEntity->base_z = g_CurrentEntity->pos_z;
        if (g_CurrentEntity == g_PlayerEntity) {
            g_RenderStateFlags[0] |= 0x80;
        }
        break;
    case 1:
        g_CurrentEntity->base_x = *argp[1];
        g_CurrentEntity->base_y = *argp[2];
        g_CurrentEntity->base_z = *argp[3];
        break;
    case 2:
        g_CurrentEntity->motion_x = *argp[1];
        g_CurrentEntity->motion_y = *argp[2];
        g_CurrentEntity->motion_z = *argp[3];
        break;
    case 3:
        g_CurrentEntity->accel_x = *argp[1];
        g_CurrentEntity->accel_y = *argp[2];
        g_CurrentEntity->accel_z = *argp[3];
        break;
    case 4:
        g_CurrentEntity->gravity_x = *argp[1];
        g_CurrentEntity->gravity_y = *argp[2];
        g_CurrentEntity->gravity_z = *argp[3];
        break;
    case 5:
        g_CurrentEntity->rot_x = *argp[1];
        g_CurrentEntity->rot_y = *argp[2];
        g_CurrentEntity->rot_z = *argp[3];
        break;
    case 6:
        g_CurrentEntity->delta_x = *argp[1];
        g_CurrentEntity->delta_y = *argp[2];
        g_CurrentEntity->delta_z = *argp[3];
        break;
    }
    return 1;
}
