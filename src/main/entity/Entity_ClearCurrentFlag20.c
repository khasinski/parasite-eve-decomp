#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearCurrentFlag20(void) {
    g_CurrentEntity->flags &= -0x21;
    return 1;
}
