#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentFlag80(void) {
    g_CurrentEntity->flags |= 0x80;
    return 1;
}

int Entity_ClearCurrentFlag80(void) {
    g_CurrentEntity->flags &= -0x81;
    return 1;
}

int Entity_SetCurrentFlag4000(void) {
    g_CurrentEntity->flags |= 0x4000;
    return 1;
}

int Entity_ClearCurrentFlag4000(void) {
    g_CurrentEntity->flags &= -0x4001;
    return 1;
}
