#include "common.h"
#include "pe1/scene_grab_trigger.h"

void SceneEffect_GrabOnApproach_801926B4(SceneGrabTrigger *trigger) {
    FieldActor *actor = trigger->actor;
    SceneGrabControl *control = &trigger->control;
    FieldActorState *state = actor->state;

    if (state != 0 && state->control10.command_value <= 1000000) {
        *trigger->control.signal = 4;
        func_80192D04((char *)trigger);
        return;
    }
    if (g_PlayerEntity->mode < 18) {
        int frame = actor->anim.parts.integer;

        if (frame < 4) {
            return;
        }
        if (frame < 12) {
            SceneGrabPoint *target = (SceneGrabPoint *)0x1F800008;

            target->x = actor->render_object.matrices[11].translation[0] << 16;
            target->z = actor->render_object.matrices[11].translation[2] << 16;
            if (func_800DFE20(&g_PlayerEntity->pos_x, &target->x) < 0x140) {
                FieldActor *player;

                control->callback = func_80192878;
                func_80020C74();
                player = g_PlayerEntity;
                player->render_object.animation_source = &actor->render_object;
                player->render_object.animation_state = 4;
                player->render_object.animation_id = 11;
                player->flags |= 0x10000;
                player->render_object.flags_9C |= 0x400;
                player->render_object.model_matrix = control->player_matrix;
                control->grabbed = 1;
                {
                    FieldActor *mover = g_PlayerEntity;

                    control->saved_x = mover->pos_x;
                    control->saved_z = mover->pos_z;
                }
            }
            return;
        }
    }
    *control->signal = 3;
    control->callback = RoomLib_RunAfterFrame37_80192BC0;
}
