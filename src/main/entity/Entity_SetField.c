#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity;

void Battle_SetContextField(int arg0, int arg1);
int Entity_SetField(char ***arg0) {
    FieldActor *ctx = g_CurrentEntity;

    if (ctx->type_id == 0) {
        Battle_SetContextField(*(unsigned char *)arg0[0], *(int *)arg0[1]);
    } else {
        Entity_WriteFieldByCmd(ctx, *(unsigned char *)arg0[0], *(int *)arg0[1]);
    }

    return 1;
}
