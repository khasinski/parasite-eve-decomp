/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"

extern FieldActor *g_CurrentEntity[];
extern int *g_TaskNodePool;

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
            g_TaskNodePool[1] = 0;
            break;
        }
    } else {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity[0]->script_cursor_1a0 = (int)(g_CurrentEntity[0]->script_base + (value << 1));
            break;
        case 2:
            g_CurrentEntity[0]->script_cursor_19c = (int)(g_CurrentEntity[0]->script_base + (value << 1));
            break;
        case 3:
            g_TaskNodePool[1] = (int)(g_CurrentEntity[0]->script_base + (value << 1));
            break;
        }
    }

    return 1;
}
