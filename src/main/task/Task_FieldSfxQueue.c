/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_sfx.h"

FieldSfxU8 g_FieldSfxQueueCount;

void Task_ClearSfxTable(void) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < 28; i++) {
        for (j = 0; j < 3; j++) {
            ((int *)&g_FieldSfxQueue[i])[j] = 0;
        }
    }
    g_FieldSfxQueueCount = 0;
}

void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4) {
    FieldSfxQueueEntry *entry = &g_FieldSfxQueue[g_FieldSfxQueueCount];

    entry->taskArgument = arg0;
    entry->subId = arg1;
    entry->taskValue = arg3;
    entry->typeId = arg2;
    entry->actorSelector.word = arg4;
    g_FieldSfxQueueCount++;
}


#include "pe1/field_sfx.h"
#include "pe1/task_node.h"


static inline void Start(FieldActor *actor, FieldSfxQueueEntry *event)
{
    TaskNode *node = Task_AllocNode(actor->script_cursor_19c, 0);

    node->flags |= 4;
    node->trigger_value = event->taskValue;
    node->target14 = event->taskArgument;
    node->next = (TaskNode *)actor->task_node_lists[2];
    if (node->next)
        node->next->prev = node;
    actor->task_node_lists[2] = node;
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
