#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

int Task_SetEntityAnimSpeed(u16 **arg0) {
    FieldActor *entity;
    unsigned int value;

    entity = g_CurrentEntity[0];
    value = **arg0;
    if (entity->action < value) {
        value = entity->action;
    }
    entity->anim.fixed = value << 16;
    return 1;
}
