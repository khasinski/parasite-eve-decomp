/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/field_actor.h"

typedef unsigned char u8;

extern FieldActor *g_CurrentEntity[];
extern FieldActorNode *g_TaskNodePool;

int Entity_MarkNodeFree(void) {
    u8 i;
    FieldActorNode *skip;
    FieldActor *entity;
    FieldActorNode *node;

    i = 0;
    entity = g_CurrentEntity[0];
    skip = g_TaskNodePool;
    do {
        node = entity->task_node_lists[i];
        while (node != 0) {
            if (node != skip) {
                node->flags |= 0x10;
            }
            node = node->next;
        }
        i++;
    } while (i < 3);

    return 1;
}
