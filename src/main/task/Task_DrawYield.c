/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Seq_GetElapsed(void);
int Gpu_CheckDrawStatus(void);

int Task_WaitFrameOrDraw(int **arg0) {
    if ((short)Seq_GetElapsed() == *arg0[0]) {
        return 1;
    }
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        g_SceneDataTable0 -= 0xC;
        g_TaskNodePool->active = 1;
    } else {
        return 1;
    }

    return 0;
}

int Task_Noop1(void) {
    return 1;
}

int Task_Noop2(void) {
    return 1;
}




int Entity_YieldOnDrawBusy(void) {
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        g_SceneDataTable0 -= 8;
        g_TaskNodePool->active = 1;
    } else {
        return 1;
    }

    return 0;
}
