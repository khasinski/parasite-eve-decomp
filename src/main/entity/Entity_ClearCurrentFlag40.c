#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearCurrentFlag40(void) {
    g_CurrentEntity->flags &= -0x41;
    return 1;
}
