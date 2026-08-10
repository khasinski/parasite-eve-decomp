/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;

int Task_JumpIfZero(int **arg0) {
    if (*arg0[0] == 0) {
        g_SceneDataTable0 = g_CurrentEntity[0]->script_base + (*arg0[1] * 2);
    }
    return 1;
}
