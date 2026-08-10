#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/task_node.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern FieldActor *g_CurrentEntity[];
extern TaskNode *g_TaskNodePool;

int Entity_MarkAnimPending(void) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    TaskNode *skip;

    i = 0;
    skip = g_TaskNodePool;
    entity = g_CurrentEntity[0];
    do {
        node = (TaskNode *)entity->task_node_lists[0];
        if (node != 0) {
            do {
                if (node != skip) {
                    node->flags |= 0x40;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (FieldActor *)((char *)entity + 4);
    } while (i < 3U);
    return 1;
}
