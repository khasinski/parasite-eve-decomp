
#include "include_asm.h"
#include "pe1/field_actor.h"
extern FieldActor *g_CurrentEntity;

int Task_GetCurrentEntityFieldD(int **arg0) {
    *arg0[0] = g_CurrentEntity->sub_id;
    return 1;
}

int Task_SetCurrentEntityFieldD(int **arg0) {
    g_CurrentEntity->sub_id = *arg0[0];
    return 1;
}

int Task_GetCurrentEntityField24(int **arg0) {
    *arg0[0] = g_CurrentEntity->field_sfx_id;
    return 1;
}
