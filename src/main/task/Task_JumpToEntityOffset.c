/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;

int Task_JumpToEntityOffset(int **arg0) {
    g_SceneDataTable0 = g_CurrentEntity[0]->field_9c + (*arg0)[0] * 2;
    return 1;
}
