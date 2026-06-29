#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearCurrentFlag80(void) {
    g_CurrentEntity->flags &= -0x81;
    return 1;
}
