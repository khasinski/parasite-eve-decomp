void Render_SetScrollMode(int arg0, int arg1, unsigned int arg2, int arg3);

extern int g_RenderStateFlags;

int Task_SetFogParams(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 0);
    return 1;
}

int Task_SetFogParamsAlt(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 8);
    return 1;
}

int Task_IsBattlePhase4(int **arg0) {
    *arg0[0] = ((g_RenderStateFlags & 7) == 4);
    return 1;
}
#include "pe1/field_actor.h"

extern FieldActor *g_PlayerEntity[];

void Render_UpdateScrollPosition(void *arg0, int arg1, int arg2);

int Task_ResetPlayerCameraPos(void) {
    Render_UpdateScrollPosition(&g_PlayerEntity[0]->pos_x, -1, -1);
    return 1;
}
extern int g_RenderStateFlags;

int Task_SetBattleStateMode(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr = (*ptr & -8) | 0x80;
    return 1;
}
#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];

int Task_SetEntityField224(short **arg0) {
    FieldActor *entity;
    int value;

    entity = g_CurrentEntity[0];
    value = (*arg0)[1];
    entity->render_object.table_value70 = value;
    return 1;
}
