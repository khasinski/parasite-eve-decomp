#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

void Render_ClearObjectAnim(void *arg0);

extern int g_BattleModeState;

int Task_ResetEntityRenderObj(void) {
    Render_ClearObjectAnim(g_CurrentEntity->attachment);
    g_CurrentEntity->parent = 0;
    return 1;
}

int Task_SetEntityField1E6(int **arg0) {
    g_CurrentEntity->render_field_1e6 = *arg0[0];
    return 1;
}

int Menu_ClearModeState(void) {
    g_BattleModeState = 0;
    return 1;
}

int Menu_SetModeState8(void) {
    g_BattleModeState = 8;
    return 1;
}
