/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;

int Entity_SetCurrentNodeActive(int **arg0) {
    g_TaskNodePool->active = *(unsigned short *)arg0[0];
    return 0;
}

int Entity_MarkCurrentNodeFree(void) {
    g_TaskNodePool->flags |= 0x10;
    return 0;
}
