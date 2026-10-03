#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"

extern FieldActor *g_PlayerEntity;
extern void func_80020CE4(void);
extern void func_80192C00(RoomPlacementOwner *owner, RoomPlacementState *state);

void func_80192838(RoomSelectionState *state) {
    FieldActor *actor = state->actor;
    RoomPlacementState *stateTail = (RoomPlacementState *)&state->callback;
    u16 currentFrame;
    u16 previousFrame;

    g_PlayerEntity->render_object.model_matrix = state->matrix;
    currentFrame = (unsigned int)actor->anim.fixed >> 16;
    previousFrame = (unsigned int)actor->anim_prev >> 16;
    g_PlayerEntity->pos_x = state->savedX;
    g_PlayerEntity->pos_z = state->savedZ;

    if (!stateTail->placement_checked) {
        FieldActorState *playerState = g_PlayerEntity->state;
        if (playerState == 0 || playerState->amount <= 0) {
            func_80192C00(actor, stateTail);
            stateTail->phase_countdown = 0;
            stateTail->placement_checked = 1;
        }
    }

    switch (actor->mode) {
    case 16:
        if ((s16)currentFrame >= 33 && (s16)previousFrame < 33) {
            actor->anim.fixed = 0x3D0000;
        } else if ((s16)currentFrame >= actor->action - 1) {
            *stateTail->signal = 2;
        }
        return;

    case 9:
        if ((s16)previousFrame < 24 && (s16)currentFrame >= 24) {
            if (stateTail->phase_countdown > 0) {
                actor->anim.fixed = 0;
                --stateTail->phase_countdown;
            }
            goto update_vertical_motion;
        }

        if ((s16)previousFrame <= 0 && (s16)currentFrame > 0) {
            u8 *substate = ((RoomM273ActorState *)actor->state)->substate;
            substate[1] = stateTail->pending_action;
            goto update_vertical_motion;
        }

        if ((s16)previousFrame < 34 && (s16)currentFrame >= 34) {
            s32 delta = ((s32)(s16)D_800942EC << 16) - actor->pos_y;
            if (delta < 0) {
                delta += 15;
            }
            stateTail->vertical_step = 4;
            stateTail->vertical_step = delta >> stateTail->vertical_step;
            stateTail->vertical_ticks = 16;
            goto update_vertical_motion;
        }

        if ((s16)previousFrame < 50 && (s16)currentFrame >= 50) {
            if (stateTail->placement_checked == 0) {
                func_80020CE4();
                func_80192C00(actor, stateTail);
            }
            goto update_vertical_motion;
        }

        if ((s16)previousFrame < 3 && (s16)currentFrame >= 3) {
            FieldActorState *playerState = g_PlayerEntity->state;
            if (playerState != 0 && playerState->amount > 0) {
                playerState->flags |= 0x4000;
                if (actor->state != 0) {
                    actor->state->core_flags |= 0x80000000;
                }
            }
            goto update_vertical_motion;
        }

        if ((s16)currentFrame >= actor->action - 1) {
            if (actor->state != 0) {
                u8 *substate = ((RoomM273ActorState *)actor->state)->substate;
                substate[0] = 4;
            }
            func_80192D8C(state);
        }
        goto update_vertical_motion;

    update_vertical_motion:
        if (stateTail->vertical_ticks != 0) {
            actor->pos_y += stateTail->vertical_step;
            --stateTail->vertical_ticks;
        }
        return;

    case 8:
        if ((s16)currentFrame <= 0) {
            FieldActorState *actorState = actor->state;
            if (actorState != 0) {
                u8 *substate = ((RoomM273ActorState *)actorState)->substate;
                substate[0] = 2;
            }
        }
        return;

    default:
        func_80192D8C(state);
        return;
    }
}
