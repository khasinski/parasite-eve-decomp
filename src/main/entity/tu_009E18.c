extern int g_GameState;

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

int Task_SetSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr |= 0x2000;
    return 1;
}

int Task_ClearSceneFlag2000(void) {
    int *ptr = &g_GameState;

    *ptr &= -0x2001;
    return 1;
}

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
