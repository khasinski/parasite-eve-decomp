/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Task_WaitAnimFrameMatch(void) {
    TaskNode *state;
    FieldActor *entity;
    int stack;
    int ret;

    state = g_TaskNodePool;
    entity = g_CurrentEntity[0];
    state->active = 1;
    ret = 0;
    if (entity->anim.parts.integer == entity->anim_frame_target) {
        return ret;
    }

    stack = g_SceneDataTable0;
    stack -= 8;
    g_SceneDataTable0 = stack;
    return ret;
}
