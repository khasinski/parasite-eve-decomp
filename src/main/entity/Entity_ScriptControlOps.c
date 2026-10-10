/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Entity_YieldWhileMoving(void) {
    int ret;

    ret = 0;
    if ((g_CurrentEntity[0]->flags & 2) == 0) {
        return 1;
    }

    g_SceneDataTable0 -= 8;
    g_TaskNodePool->active = 1;
    return ret;
}
int Entity_SelectFieldSetter(int **arg0) {
    int value;
    int selector;

    value = *arg0[1];
    if (value < 0) {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity[0]->script_cursor_1a0 = 0;
            break;
        case 2:
            g_CurrentEntity[0]->script_cursor_19c = 0;
            break;
        case 3:
            g_TaskNodePool->next_value = 0;
            break;
        }
    } else {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity[0]->script_cursor_1a0 = (g_CurrentEntity[0]->script_base + (value << 1));
            break;
        case 2:
            g_CurrentEntity[0]->script_cursor_19c = (g_CurrentEntity[0]->script_base + (value << 1));
            break;
        case 3:
            g_TaskNodePool->next_value = (int)(g_CurrentEntity[0]->script_base + (value << 1));
            break;
        }
    }

    return 1;
}

int Entity_SetCurrentFlags(int **args)
{
    g_CurrentEntity[0]->flags |= (*args)[0];
    return 1;
}

int Entity_GetCurrentNodeId(int **args)
{
    *args[0] = g_TaskNodePool->seq;
    return 1;
}
