/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
int g_SceneDataTable0;

int Task_CondBranch(int **arg0) {
    int *a;
    int *b;
    int *c;

    a = arg0[0];
    b = arg0[1];
    if (*a != *b) {
        c = arg0[2];
        g_SceneDataTable0 = (int)(g_CurrentEntity[0]->script_base + (*c * 2));
    }
    return 1;
}
