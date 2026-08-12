#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/task_runtime.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

int Task_PlayEntityAnim(int **args) {
    TaskNode *task;
    unsigned short flags;

    task = g_CurrentTaskNode;
    flags = task->flags;

    if (flags & 0x20) {
        if (!(g_CurrentEntity->render_object.flags_9C & 4)) {
            task->flags = flags & ~0x20;
            return 1;
        }
    } else {
        g_CurrentEntity->flags &= ~0x40;
        task->flags |= 0x20;

        Anim_SetInterpRate(&g_CurrentEntity->render_object, (short)*args[0]);
        g_CurrentEntity->render_object.flags_9C |= 4;

        if (g_CurrentEntity == g_PlayerEntity) {
            D_8009D2E8 &= ~2;
            Anim_SetInterpRate(&g_PlayerBattleRenderObject, (short)*args[0]);
            g_PlayerBattleRenderObject.flags_9C |= 4;
        }
    }

    g_TaskScriptCursor -= 12;
    g_CurrentTaskNode->active = 1;
    return 0;
}
