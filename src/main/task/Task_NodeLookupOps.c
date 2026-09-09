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
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;
extern char *g_CurrentEntity[];

int Entity_FindNodeById(unsigned short **arg0) {
    TaskNode *node;
    TaskNode *cur;
    register unsigned int id asm("$2");
    unsigned int bucket;
    unsigned int search_id;
    char *base;

    node = g_TaskNodePool;
    asm volatile("" : "=r"(node) : "0"(node));
    id = *arg0[0];
    if (node->seq == id) {
        node->flags |= 0x10;
        return 0;
    }

    base = g_CurrentEntity[0];
    search_id = id;
    bucket = 0;
    do {
        cur = *(TaskNode **)(base + 0xA0 + ((bucket & 0xFF) * 4));
        while (cur != 0) {
            if (cur->seq == search_id) {
                cur->flags |= 0x10;
                return 1;
            }
            cur = cur->next;
        }
        bucket++;
    } while ((bucket & 0xFF) < 3);

    return 1;
}
