/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

typedef unsigned short u16;

extern int g_TaskNodePool;
extern int g_TaskNodeFreeListHead;
extern u16 g_TaskNodeSeqCounter;
extern int *g_CurrentEntity[];

int Task_SpawnChildNode(int **args) {
    TaskNode *node = (TaskNode *)g_TaskNodePool;

    if ((node->flags & 3) != 0) {
        register int *value_ptr asm("$2");
        register int *state asm("$5");
        TaskNode *entry;
        u16 seq;
        register int value asm("$4");
        int base;
        TaskNode *next;

        value_ptr = args[0];
        state = g_CurrentEntity[0];
        entry = (TaskNode *)g_TaskNodeFreeListHead;
        asm volatile("" : : "r"(entry));
        seq = g_TaskNodeSeqCounter;
        value = *value_ptr;
        base = state[0x9C / 4];
        asm volatile("lw\t%0,0x24(%1)" : "=r"(next) : "r"(entry));
        entry->prev = 0;
        entry->next = 0;
        entry->field_0c = 0;
        entry->field_04 = 0;
        entry->field_10 = 1;
        entry->seq = seq;
        entry->flags = 0;
        value <<= 1;
        value += base;
        entry->ptr = value;
        {
            TaskNode *head = (TaskNode *)state[0xA8 / 4];
            seq++;
            g_TaskNodeSeqCounter = seq;
            g_TaskNodeFreeListHead = (int)next;
            entry->next = head;
            if (head != 0) {
                head->prev = entry;
            }
            ((TaskNode **)g_CurrentEntity[0])[0xA8 / 4] = entry;
        }
    } else {
        register TaskNode *entry asm("$5") = (TaskNode *)g_TaskNodeFreeListHead;
        register int *value_ptr asm("$2") = args[0];
        register int *state asm("$4") = g_CurrentEntity[0];
        TaskNode *next = entry->next;
        int value;

        value = *value_ptr;
        g_TaskNodeFreeListHead = (int)next;
        asm volatile("" : : : "memory");
        value = (value << 1) + state[0x9C / 4];
        if (node != 0) {
            entry->prev = node;
            next = node->next;
            entry->next = next;
            if (next != 0) {
                next->prev = entry;
            }
            node->next = entry;
        } else {
            register int zero asm("$0");
            entry->prev = (TaskNode *)zero;
            entry->next = (TaskNode *)zero;
        }
        entry->ptr = value;
        {
            u16 seq = g_TaskNodeSeqCounter;
            entry->field_0c = 0;
            entry->field_04 = 0;
            entry->field_10 = 1;
            entry->flags = 0;
            g_TaskNodeSeqCounter = seq + 1;
            entry->seq = seq;
        }
    }
    return 1;
}
