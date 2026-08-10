#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_CurrentEntity[];

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
