#include "common.h"
#include "pe1/field_actor.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct GlobalFlagSlot {
    u32 value;
    u8 pad[8];
} GlobalFlagSlot;

extern FieldActor *current_entity_wait[] __asm__("D_8009D2F0");
extern FieldActor *current_entity_start[] __asm__("D_8009D2F0");
extern FieldActor *current_entity_post[] __asm__("D_8009D2F0");
extern FieldActor *g_PlayerEntity[] __asm__("D_8009D254");
extern GlobalFlagSlot entity_control_load __asm__("D_8009D2E8");
extern GlobalFlagSlot entity_control_store __asm__("D_8009D2E8");
extern FieldActorState *g_CurrentTaskState __asm__("D_8009D300");
extern s16 *g_ScriptCursor __asm__("D_8009CE00");
extern RenderObjectEntity D_800B0CEC;

void Anim_SetInterpRate(RenderObjectEntity *object, int rate);

int Task_PlayEntityAnim(s16 **args) {
    FieldActorState *task_state;
    FieldActor *actor;
    FieldActor *post_actor;
    FieldActor *player;
    u32 control_flags;
    u16 progress;

    task_state = g_CurrentTaskState;
    progress = *(u16 *)&task_state->progress;
    if (progress & 0x20) {
        if (!(current_entity_wait[0]->render_object.flags_9C & 4)) {
            *(u16 *)&task_state->progress = progress & ~0x20;
            return 1;
        }
    } else {
        actor = current_entity_start[0];
        actor->flags &= ~0x40;
        *(u16 *)&task_state->progress |= 0x20;
        Anim_SetInterpRate(&actor->render_object, *args[0]);
        post_actor = current_entity_post[0];
        player = g_PlayerEntity[0];
        post_actor->render_object.flags_9C |= 4;

        if (post_actor == player) {
            control_flags = entity_control_load.value;
            control_flags &= ~2;
            entity_control_store.value = control_flags;
            Anim_SetInterpRate(&D_800B0CEC, *args[0]);
            D_800B0CEC.flags_9C |= 4;
        }
    }

    g_ScriptCursor -= 6;
    g_CurrentTaskState->control10.command_value = 1;
    return 0;
}
