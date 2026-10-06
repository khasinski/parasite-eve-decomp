/* Script opcodes: no-op, animation step and animation speed of the current
 * actor. Contiguous default-profile handlers. */
#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

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
