#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

void Entity_SetActionMode(FieldActor *entity, int arg1);

int Entity_SetCurrentActionMode(u16 **arg0) {
    FieldActor *entity;

    Entity_SetActionMode(g_CurrentEntity, *arg0[0]);
    entity = g_CurrentEntity;
    entity->flags &= ~0x100;
    return 1;
}
#include "common.h"
#include "pe1/field_actor.h"



int Task_SetEntityAnimFrame(u16 **arg0) {
    FieldActor *entity = g_CurrentEntity;
    u16 value = *arg0[0];

    if (entity->action < value) {
        value = entity->action;
    }

    entity->anim_frame_target = value;
    entity->flags |= 0x200;
    return 1;
}
