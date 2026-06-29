/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern int *g_TaskNodePool;

int Entity_YieldWhileMoving(void) {
    int ret;

    ret = 0;
    if ((g_CurrentEntity[0]->flags & 2) == 0) {
        return 1;
    }

    g_SceneDataTable0 -= 8;
    g_TaskNodePool[4] = 1;
    return ret;
}
