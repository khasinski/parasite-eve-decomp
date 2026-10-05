/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;

int Entity_GetCurrentNodeId(int **arg0) {
    *arg0[0] = g_TaskNodePool->seq;
    return 1;
}
