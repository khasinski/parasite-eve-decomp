#ifndef PE1_TASK_QUEUE_H
#define PE1_TASK_QUEUE_H

#include "common.h"
#include "pe1/field_actor.h"

#include "pe1/task_node.h"

extern u32 *D_8009CE00;
extern u8 *D_8009D2F0[];
extern u8 *D_8009D254[];
extern u32 D_8009D1A0[];
extern u32 D_8009DF70[], D_800A77F0[], D_800B6A80[];
extern int (*D_800910A0[])(u32 **);

void Task_RunQueue(void);

#endif
