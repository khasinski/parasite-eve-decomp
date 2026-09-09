#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/task_node.h"

extern TaskNode *g_TaskNodePool;
extern TaskNode *g_TaskNodeFreeListHead;
extern u16 g_TaskNodeSeqCounter;
extern char *g_CurrentEntity[];

int Task_SpawnChildNode(int **args) {
    TaskNode *node = g_TaskNodePool;

    if ((node->flags & 3) != 0) {
        register int *value_ptr asm("$2");
        register int *state asm("$5");
        TaskNode *entry;
        u16 seq;
        register int value asm("$4");
        int base;
        TaskNode *next;

        value_ptr = args[0];
        state = (int *)g_CurrentEntity[0];
        entry = g_TaskNodeFreeListHead;
        seq = g_TaskNodeSeqCounter;
        value = *value_ptr;
        base = state[0x9C / 4];
        asm volatile("" : "=r"(entry) : "0"(entry), "r"(value), "r"(base));
        next = entry->next;
        entry->prev = 0;
        entry->next = 0;
        entry->field_0c = 0;
        entry->next_value = 0;
        entry->active = 1;
        entry->seq = seq;
        entry->flags = 0;
        asm volatile("" : "=r"(value) : "0"(value));
        value <<= 1;
        value += base;
        entry->current = value;
        {
            TaskNode *head = (TaskNode *)state[0xA8 / 4];
            seq++;
            g_TaskNodeSeqCounter = seq;
            g_TaskNodeFreeListHead = next;
            entry->next = head;
            if (head != 0) {
                head->prev = entry;
            }
            ((TaskNode **)g_CurrentEntity[0])[0xA8 / 4] = entry;
        }
    } else {
        register TaskNode *entry asm("$5") = g_TaskNodeFreeListHead;
        register int *value_ptr asm("$2") = args[0];
        register int *state asm("$4") = g_CurrentEntity[0];
        TaskNode *next = entry->next;
        int value;

        value = *value_ptr;
        g_TaskNodeFreeListHead = next;
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
            entry->prev = 0;
            entry->next = 0;
        }
        entry->current = value;
        {
            u16 seq = g_TaskNodeSeqCounter;
            entry->field_0c = 0;
            entry->next_value = 0;
            entry->active = 1;
            entry->flags = 0;
            g_TaskNodeSeqCounter = seq + 1;
            entry->seq = seq;
        }
    }
    return 1;
}

extern int g_SceneDataTable0;
extern TaskNode *g_TaskNodePool;
extern char *g_CurrentEntity[];
/* Same symbol; keep the post-call address calculation independent in GCC. */
extern char *g_CurrentEntityAfterAction[] asm("g_CurrentEntity");

void Entity_SetActionMode(char *arg0, int arg1);

int Task_SetEntityActionAndWait(int **arg0) {
    char *node = (char *)g_TaskNodePool;
    int flags = *(u16 *)(node + 8);

    if ((flags & 0x20) == 0) {
        int *ptr;
        int mode;

        *(u16 *)(node + 8) = flags | 0x20;
        ptr = arg0[0];
        {
            char *state;
            state = g_CurrentEntity[0];
            mode = *(u16 *)ptr;
            Entity_SetActionMode(state, mode);
        }
        {
            char *state;
            state = g_CurrentEntityAfterAction[0];
            *(int *)(state + 0x98) &= -0x101;
        }
        goto pop_state;
    } else {
        char *state;
        int keep;

        state = g_CurrentEntity[0];
        if (*(u8 *)(state + 0xF) == 0) {
            goto finish;
        }
        if (*(int *)(state + 0x1C) >= 0) {
            register unsigned int lhs asm("$2") = *(unsigned int *)(state + 0x14);
            unsigned int rhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        } else {
            unsigned int rhs = *(unsigned int *)(state + 0x14);
            unsigned int lhs = *(unsigned int *)(state + 0x18);
            int result = lhs < rhs;
            keep = result;
        }
        if (keep == 0) {
            goto pop_state;
        }

finish:
        *(u16 *)(node + 8) = flags & 0xFFDF;
        return 1;
    }

pop_state:
    {
        int cursor = g_SceneDataTable0;
        int *active_node = (int *)g_TaskNodePool;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        active_node[4] = 1;
        return 0;
    }
}
