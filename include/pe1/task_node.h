#ifndef PE1_TASK_NODE_H
#define PE1_TASK_NODE_H

/* Task/scene node: elements of the free list (D_8009CDFC), the current-node
 * pointer (g_TaskNodePool @ 0x8009D300), and the per-entity bucket lists
 * (FieldActor.task_node_lists @ 0xA0). Doubly-linked via next/prev. Size 0x2C. */
typedef struct TaskNode {
    /* 0x00 */ int ptr;
    /* 0x04 */ int field_04;
    /* 0x08 */ unsigned short flags;   /* |= 0x10 marks node free */
    /* 0x0A */ unsigned short seq;      /* sequence/id, matched in Entity_FindNodeById */
    /* 0x0C */ int field_0c;
    /* 0x10 */ int field_10;            /* set 1 on alloc */
    /* 0x14 */ int field_14;
    /* 0x18 */ int field_18;
    /* 0x1C */ int field_1c;
    /* 0x20 */ int field_20;
    /* 0x24 */ struct TaskNode *next;
    /* 0x28 */ struct TaskNode *prev;
} TaskNode;

#endif /* PE1_TASK_NODE_H */
