#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag4000(void) {
    g_CurrentEntity->flags |= 0x4000;
    return 1;
}
