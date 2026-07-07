/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

int g_FieldMoveLock;
FieldActor **g_PlayerEntity;
extern char D_800943C0[];

void Entity_DispatchCallbacks(FieldActor *a);
void Scene_CheckFlagBits(FieldActor *a, char *arg1, int *arg2);

void Entity_IntegratePositionFull(FieldActor *a)
{
    int tmp;
    int v0;

    a->base_x = a->pos_x;
    a->base_y = a->pos_y;
    a->base_z = a->pos_z;
    a->saved_rot_x = a->rot_x;
    a->saved_rot_y = a->rot_y;
    a->saved_rot_z = a->rot_z;
    Entity_DispatchCallbacks(a);

    if ((g_FieldMoveLock & 1) == 0) {
        v0 = g_PlayerEntity[0]->field_4c;
        tmp = a->mode;
        if ((v0 & 0xC0) == 0x80) {
            tmp = 0x11;
        }
        Scene_CheckFlagBits(a, D_800943C0, &tmp);
    }

    if (a->flags & 2) {
        a->motion_x += a->gravity_x;
        a->motion_y += a->gravity_y;
        a->motion_z += a->gravity_z;
    }

    a->motion_x += a->accel_x;
    a->motion_y += a->accel_y;
    a->motion_z += a->accel_z;

    a->pos_x += a->motion_x;
    a->pos_y += a->motion_y;
    a->pos_z += a->motion_z;

    a->pos_x += a->delta_x;
    a->pos_y += a->delta_y;
    a->pos_z += a->delta_z;
}
