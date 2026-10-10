#include "pe1/field_actor.h"
#include "pe1/global_slot.h"


extern u8 *D_8009D20C[];
extern Pe1GlobalSlot D_8009D254;

int Entity_GetPositionByType(int **args) {
    FieldActor *entity;
    FieldActor *player;
    int selector;
    int entity_id;

    selector = *args[1];
    if (selector == 0) {
        player = (FieldActor *)D_8009D254.value.pointer;
        if (player == 0) {
            *args[6] = -1;
            return 1;
        }
        entity = player;
    } else {
        entity = (FieldActor *)D_8009D20C[0];
        entity_id = selector;
        while (entity != 0) {
            if (entity->type_id == entity_id &&
                entity->sub_id == *args[2] &&
                (entity->flags & 0x10) == 0) {
                break;
            }
            entity = entity->next;
        }
        if (entity == 0) {
            *args[6] = -1;
            return 1;
        }
    }

    *args[6] = 1;
    switch (*args[0]) {
    case 0:
        *args[3] = entity->pos_x;
        *args[4] = entity->pos_y;
        *args[5] = entity->pos_z;
        break;
    case 1:
        *args[3] = entity->base_x;
        *args[4] = entity->base_y;
        *args[5] = entity->base_z;
        break;
    case 2:
        *args[3] = entity->motion_x;
        *args[4] = entity->motion_y;
        *args[5] = entity->motion_z;
        break;
    case 3:
        *args[3] = entity->accel_x;
        *args[4] = entity->accel_y;
        *args[5] = entity->accel_z;
        break;
    case 4:
        *args[3] = entity->gravity_x;
        *args[4] = entity->gravity_y;
        *args[5] = entity->gravity_z;
        break;
    case 5:
        *args[3] = entity->rot_x;
        *args[4] = entity->rot_y;
        *args[5] = entity->rot_z;
        break;
    case 6:
        *args[3] = entity->delta_x;
        *args[4] = entity->delta_y;
        *args[5] = entity->delta_z;
        break;
    }
    return 1;
}
