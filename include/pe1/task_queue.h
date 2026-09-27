#ifndef PE1_TASK_QUEUE_H
#define PE1_TASK_QUEUE_H

#include "common.h"
#include "pe1/field_actor.h"

/* Runtime view of a 0x2C-byte task node. The dispatcher reads flags as a
 * word initially, then tests the low halfword while checking pause gates. */
typedef struct QueueNode {
    u32 *script;
    u32 next_value;
    union { u32 word; u16 half[2]; } flags;
    u32 field_0c;
    u32 ticks;
    u32 targets[4];
    struct QueueNode *next;
    struct QueueNode *prev;
} QueueNode;

extern QueueNode *D_8009D300;
extern u32 *D_8009CE00;
extern u8 *D_8009D2F0[];
extern u8 *D_8009D254[];
extern u32 D_8009D1A0[];
extern u32 D_8009DF70[], D_800A77F0[], D_800B6A80[];
extern int (*D_800910A0[])(u32 **);

void Task_RunQueue(void);

#endif
