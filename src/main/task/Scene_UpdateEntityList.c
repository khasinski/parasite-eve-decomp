/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_sfx.h"
#include "pe1/task_node.h"

FieldSfxU8 g_FieldSfxQueueCount;

static inline void Start(FieldActor *actor, FieldSfxQueueEntry *event)
{
    TaskNode *node = Task_AllocNode(actor->script_cursor_19c, 0);

    node->flags |= 4;
    node->field_0c = event->taskValue;
    node->target14 = event->taskArgument;
    node->next = (TaskNode *)actor->task_node_lists[2];
    if (node->next)
        node->next->prev = node;
    actor->task_node_lists[2] = (struct FieldActorNode *)node;
}

void Scene_UpdateEntityList(void)
{
    unsigned char i;

    for (i = 0; i < g_FieldSfxQueueCount; i++) {
        FieldSfxQueueEntry *event = &g_FieldSfxQueue[i];
        FieldActor *actor;

        if (event->actorSelector.id) {
            unsigned int id = event->actorSelector.id;
            actor = g_FieldActorListHead;
            while (actor) {
                if (actor->field_sfx_id == id) {
                    if (actor->script_cursor_19c)
                        Start(actor, event);
                    actor = 0;
                } else {
                    actor = actor->next;
                }
            }
        } else {
            actor = g_FieldActorListHead;
            while (actor) {
                if (actor->type_id == event->typeId &&
                    actor->script_cursor_19c && actor->sub_id == event->subId)
                    Start(actor, event);
                actor = actor->next;
            }
        }
    }
    g_FieldSfxQueueCount = 0;
}
