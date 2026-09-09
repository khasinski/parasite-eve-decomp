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


int Entity_AdvancePendingAnim(void) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    int active;

    i = 0;
    active = 1;
    entity = g_CurrentEntity[0];
    do {
        node = (TaskNode *)entity->task_node_lists[0];
        if (node != 0) {
            do {
                if (node->flags & 0x40) {
                    node->flags &= ~0x40;
                    if (node->next_value != 0) {
                        node->current = node->next_value;
                        node->active = active;
                        node->flags &= ~0x20;
                    }
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (FieldActor *)((char *)entity + 4);
    } while (i < 3U);
    return 1;
}


int Entity_MarkNodeAnimPendingById(int **arg0) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;

    i = 0;
    entity = g_CurrentEntity[0];
    do {
        node = (TaskNode *)entity->task_node_lists[0];
        if (node != 0) {
            do {
                if (node->seq == **arg0) {
                    node->flags |= 0x40;
                    return 1;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (FieldActor *)((char *)entity + 4);
    } while (i < 3U);

    return 1;
}


int Entity_AdvanceNodeById(int **arg0) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    int next_value;
    int active_value;

    i = 0;
    active_value = 1;
    entity = g_CurrentEntity[0];
    do {
        node = (TaskNode *)entity->task_node_lists[0];
        if (node != 0) {
            do {
                if (node->seq == **arg0) {
                    node->flags &= ~0x40;
                    next_value = node->next_value;
                    if (next_value != 0) {
                        node->current = next_value;
                        node->active = active_value;
                        node->flags &= ~0x20;
                    }
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        entity = (FieldActor *)((char *)entity + 4);
    } while (i < 3U);

    return 1;
}
