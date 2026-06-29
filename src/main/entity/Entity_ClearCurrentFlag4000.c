#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearCurrentFlag4000(void) {
    g_CurrentEntity->flags &= -0x4001;
    return 1;
}
