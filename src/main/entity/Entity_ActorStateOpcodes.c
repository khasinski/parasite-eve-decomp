/* Contiguous current-actor flag and animation opcodes. */
#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

int Entity_TestCurrentFlags(int **arg0) {
    FieldActor *entity;
    int flags;
    int mask;

    entity = g_CurrentEntity[0];
    flags = entity->flags;
    mask = *arg0[0];
    *arg0[1] = (flags & mask) == mask;
    return 1;
}

int Task_Noop(void) {
    return 1;
}

int Task_SetEntityField1C(int **arg0) {
    g_CurrentEntity[0]->anim_step = *arg0[0];
    return 1;
}

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

int Entity_SetCurrentFlag100(void) {
    g_CurrentEntity[0]->flags |= 0x100;
    return 1;
}

int Entity_ClearCurrentFlag100(void) {
    g_CurrentEntity[0]->flags &= -0x101;
    return 1;
}
