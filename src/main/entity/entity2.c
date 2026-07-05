extern int g_FieldMoveLock;

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Entity_ClearGlobalFlag1(void) {
    g_FieldMoveLock &= -2;
    return 1;
}

int Entity_SetGlobalFlag1(void) {
    g_FieldMoveLock |= 1;
    return 1;
}

int Entity_SetCurrentFlag40(void) {
    g_CurrentEntity->flags |= 0x40;
    return 1;
}

int Entity_ClearCurrentFlag40(void) {
    g_CurrentEntity->flags &= -0x41;
    return 1;
}
