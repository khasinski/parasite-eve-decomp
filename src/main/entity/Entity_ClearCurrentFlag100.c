#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearCurrentFlag100(void) {
    g_CurrentEntity->flags &= -0x101;
    return 1;
}
