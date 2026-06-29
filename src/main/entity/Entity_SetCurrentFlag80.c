#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag80(void) {
    g_CurrentEntity->flags |= 0x80;
    return 1;
}
