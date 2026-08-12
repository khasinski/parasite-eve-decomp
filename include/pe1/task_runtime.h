#ifndef PE1_TASK_RUNTIME_H
#define PE1_TASK_RUNTIME_H

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

extern FieldActor *g_PlayerEntity __asm__("D_8009D254");
extern FieldActor *g_CurrentEntity __asm__("D_8009D2F0");
extern TaskNode *g_CurrentTaskNode __asm__("D_8009D300");
extern unsigned char *g_TaskScriptCursor __asm__("D_8009CE00");
extern RenderObjectEntity g_PlayerBattleRenderObject __asm__("D_800B0CEC");

#endif /* PE1_TASK_RUNTIME_H */
