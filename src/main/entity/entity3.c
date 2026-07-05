#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag100(void) {
    g_CurrentEntity->flags |= 0x100;
    return 1;
}

int Entity_ClearCurrentFlag100(void) {
    g_CurrentEntity->flags &= -0x101;
    return 1;
}
