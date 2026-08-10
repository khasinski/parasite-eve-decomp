
#include "include_asm.h"
#include "pe1/field_actor.h"
extern int g_RenderStateFlags;

extern FieldActor *g_CurrentEntity;

int Task_ClearBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr &= -0xC1;
    return 1;
}

int Task_SetBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr |= 0xC0;
    return 1;
}

int Task_SetCurrentEntityField20(int **arg0) {
    g_CurrentEntity->move_factor = *arg0[0];
    return 1;
}
