/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern unsigned char g_ScreenTransitionState[];
extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Entity_YieldOnLoading(void) {
    if ((g_ScreenTransitionState[0] & 3) >= 2) {
        g_SceneDataTable0 -= 8;
        g_TaskNodePool->active = 1;
        return 0;
    }

    return 1;
}
