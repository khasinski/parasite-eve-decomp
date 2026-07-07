
#include "pe1/field_actor.h"

typedef unsigned short u16;

extern FieldActor *g_CurrentEntity[];

int Task_SetEntityAnimFrame(u16 **arg0) {
    FieldActor *entity = g_CurrentEntity[0];
    u16 value = *arg0[0];

    if (entity->action < value) {
        value = entity->action;
    }

    entity->anim_frame_target = value;
    entity->flags |= 0x200;
    return 1;
}
