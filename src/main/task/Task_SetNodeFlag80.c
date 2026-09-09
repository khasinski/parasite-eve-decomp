/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;

int Task_SetNodeFlag80(void) {
    g_TaskNodePool->flags |= 0x80;
    return 1;
}
