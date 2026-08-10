#include "pe1/field_actor.h"

extern FieldActor *g_PlayerEntity[];

void Render_UpdateScrollPosition(void *arg0, int arg1, int arg2);

int Task_ResetPlayerCameraPos(void) {
    Render_UpdateScrollPosition(&g_PlayerEntity[0]->pos_x, -1, -1);
    return 1;
}
