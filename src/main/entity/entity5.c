#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/task_node.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef FieldActor Entity;

Entity *g_CurrentEntity;
extern int g_TaskNodePool[];

void Task_RunQueue(void);

void Entity_DispatchCallbacks(Entity *arg0) {
    int i;
    int *ptr;
    int callback;

    i = 0;
    ptr = (int *)arg0->task_node_lists;
    g_CurrentEntity = arg0;
    do {
        callback = *ptr;
        g_TaskNodePool[0] = callback;
        if (callback != 0) {
            Task_RunQueue();
        }
        i++;
        ptr++;
    } while ((unsigned int)i < 3U);
}

void Entity_TickAnimSequences(FieldActor *arg0) {
    unsigned int i;
    TaskNode *node;

    i = 0;
    do {
        node = (TaskNode *)arg0->task_node_lists[0];
        if (node != 0) {
            do {
                if (node->next_value != 0) {
                    node->current = node->next_value;
                    node->active = 1;
                    node->flags &= ~0x60;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        arg0 = (FieldActor *)((char *)arg0 + 4);
    } while (i < 3U);
}
