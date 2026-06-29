/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern char *g_TaskNodePool;

int Task_WaitAnimFrameMatch(void) {
    int *state;
    FieldActor *entity;
    int stack;
    int ret;

    state = (int *)g_TaskNodePool;
    entity = g_CurrentEntity[0];
    state[4] = 1;
    ret = 0;
    if (entity->anim_frame == entity->anim_frame_target) {
        return ret;
    }

    stack = g_SceneDataTable0;
    stack -= 8;
    g_SceneDataTable0 = stack;
    return ret;
}
