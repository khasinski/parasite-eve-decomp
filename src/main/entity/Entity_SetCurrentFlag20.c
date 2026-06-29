#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag20(void) {
    g_CurrentEntity->flags |= 0x20;
    return 1;
}
