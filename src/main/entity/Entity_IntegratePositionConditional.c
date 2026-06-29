#include "pe1/field_actor.h"

extern int g_GameStateFlags;

void Entity_DispatchCallbacks(FieldActor *a);

void Entity_IntegratePositionConditional(FieldActor *a)
{
    if (g_GameStateFlags & 0x100) {
        Entity_DispatchCallbacks(a);
        return;
    }

    a->base_x = a->pos_x;
    a->base_y = a->pos_y;
    a->base_z = a->pos_z;
    a->saved_rot_x = a->rot_x;
    a->saved_rot_y = a->rot_y;
    a->saved_rot_z = a->rot_z;
    Entity_DispatchCallbacks(a);

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
