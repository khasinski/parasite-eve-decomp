/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
    asm volatile("nop");
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
    node->field_0c = 0;
    node->ptr = arg0;
    node->field_04 = 0;
    node->field_10 = one;
    node->flags = 0;
    one = seq + 1;
    node->seq = seq;
    g_TaskNodeSeqCounter = one;

    return node;
}
