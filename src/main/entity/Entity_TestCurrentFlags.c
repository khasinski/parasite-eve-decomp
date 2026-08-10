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
