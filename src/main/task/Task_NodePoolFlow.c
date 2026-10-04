/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --dont-expand-li */
/* ASPSX_VERSION: 2.21 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodeFreeListHead;
extern unsigned short g_TaskNodeSeqCounter;

TaskNode *Task_AllocNode(int arg0, TaskNode *arg1) {
    TaskNode *node;
    TaskNode *next;
    unsigned short seq;
    register int one asm("$3");

    node = g_TaskNodeFreeListHead;
    next = node->next;
    g_TaskNodeFreeListHead = next;

    if (arg1 != 0) {
        node->prev = arg1;
        next = arg1->next;
        node->next = next;
        if (next != 0) {
            next->prev = node;
        }
        arg1->next = node;
    } else {
        node->prev = 0;
        node->next = 0;
    }

    seq = g_TaskNodeSeqCounter;
    one = 1;
    node->trigger_value = 0;
    node->current = arg0;
    node->next_value = 0;
    node->active = one;
    node->flags = 0;
    one = seq + 1;
    node->seq = seq;
    g_TaskNodeSeqCounter = one;

    return node;
}

#include "common.h"
#include "pe1/task_node.h"

typedef struct Group {
    char pad0[4];
    struct Group *nextGroup;
    char pad8[0x90];
    int flags;
    char pad9C[4];
    TaskNode *buckets[3];
} Group;

extern Group *g_FieldActorListHead[];
extern TaskNode *g_TaskNodeFreeListHead;

void Task_GarbageCollectNodes(void) {
    Group *group;
    u8 i;

    group = g_FieldActorListHead[0];
    if (group != 0) {
        do {
            i = 0;
            do {
                TaskNode *node;
                TaskNode *next;
                register char *bucketBase;

                bucketBase = (char *)(((unsigned int)i * 4) + (unsigned int)group);
                node = *(TaskNode **)(bucketBase + 0xA0);
                if (node != 0) {
                    char *bucket;

                    bucket = bucketBase;
                    do {
                        next = node->next;
                        if ((group->flags & 0x10) || (node->flags & 0x10)) {
                            if (node->prev != 0) {
                                node->prev->next = next;
                            } else {
                                *(TaskNode **)(bucket + 0xA0) = next;
                            }

                            if (node->next != 0) {
                                node->next->prev = node->prev;
                            }

                            node->next = g_TaskNodeFreeListHead;
                            g_TaskNodeFreeListHead = node;
                        }

                        node = next;
                    } while (node != 0);
                }

                i++;
            } while (i < 3);

            group = group->nextGroup;
        } while (group != 0);
    }
}
