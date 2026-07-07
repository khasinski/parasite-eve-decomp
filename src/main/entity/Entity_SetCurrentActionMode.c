
#include "pe1/field_actor.h"

typedef unsigned short u16;

extern FieldActor *g_CurrentEntity;

void Entity_SetActionMode(FieldActor *entity, int arg1);

int Entity_SetCurrentActionMode(u16 **arg0) {
    FieldActor *entity;

    Entity_SetActionMode(g_CurrentEntity, *arg0[0]);
    entity = g_CurrentEntity;
    entity->flags &= ~0x100;
    return 1;
}
