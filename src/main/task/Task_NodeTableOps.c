/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;

int Tbl_LookupEntry(short arg0);
void Tbl_ClearEntry(int arg0);

int Task_GetNodeTarget14(int **arg0) {
    *arg0[0] = g_TaskNodePool->target14;
    return 1;
}

int Task_YieldWhileTableEntrySet(short **arg0) {
    if ((Tbl_LookupEntry(*arg0[0]) << 24) != 0) {
        g_SceneDataTable0 -= 0xC;
        g_TaskNodePool->active = 1;
        return 0;
    }
    return 1;
}

int Task_ClearTableEntry(short **arg0) {
    Tbl_ClearEntry(*arg0[0]);
    return 1;
}

int Task_GetNodeTargets18_1C(int **arg0) {
    *arg0[0] = g_TaskNodePool->target18;
    *arg0[1] = g_TaskNodePool->target1c;
    return 1;
}
