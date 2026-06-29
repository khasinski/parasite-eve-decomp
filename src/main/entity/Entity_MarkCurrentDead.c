#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_MarkCurrentDead(void) {
    g_CurrentEntity->flags |= 0x10;
    return 0;
}
