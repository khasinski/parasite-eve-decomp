#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag40(void) {
    g_CurrentEntity->flags |= 0x40;
    return 1;
}
