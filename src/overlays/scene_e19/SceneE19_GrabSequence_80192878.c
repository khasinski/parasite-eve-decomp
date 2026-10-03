#include "pe1/scene_e19_selection.h"

/* Grab sequence driver: pins the player to the selection matrix and steps
 * the partner's animation events. Both frame numbers are the integer halves
 * of the 16.16 positions taken with an unsigned shift, which gives retail's
 * lhu loads and the copies cse keeps for the later cases. */

void func_80192878(SceneE19Selection *selection) {
    FieldActor *actor = selection->actor;
    SceneE19SelectionTail *tail = &selection->tail;
    FieldActorState *playerState;
    SceneE19ActorState *actorState;
    u16 currentFrame;
    u16 previousFrame;

    g_PlayerEntity->render_object.model_matrix = selection->matrix;
    currentFrame = (unsigned int)actor->anim.fixed >> 16;
    previousFrame = (unsigned int)actor->anim_prev >> 16;
    g_PlayerEntity->pos_x = selection->savedX;
    g_PlayerEntity->pos_z = selection->savedZ;
    if (g_PlayerEntity->state == 0 || g_PlayerEntity->state->amount <= 0) {
        func_80192B10(actor, tail);
        func_80192BFC(selection);
        return;
    }

    switch (actor->mode) {
    case 16:
        if ((s16)currentFrame >= 11 && (s16)previousFrame < 11) {
            actor->anim.fixed = 0x280000;
        } else if ((s16)currentFrame >= actor->action - 1) {
            *tail->signal = 2;
        }
        return;
    case 13:
        actorState = (SceneE19ActorState *)actor->state;
        if (actorState != 0 && *actorState->substate == 1) {
            *actorState->substate = 2;
        }
        if ((s16)previousFrame < 18 && (s16)currentFrame >= 18) {
            func_80020CE4();
            func_80192B10(actor, tail);
            return;
        }
        if ((s16)previousFrame < 3 && (s16)currentFrame >= 3) {
            playerState = g_PlayerEntity->state;
            if (playerState != 0 && playerState->amount > 0) {
                playerState->flags |= 0x4000;
                if (actor->state != 0) {
                    actor->state->core_flags |= 0x80000000;
                }
            }
            return;
        }
        if ((s16)currentFrame < actor->action - 1) {
            return;
        }
        actorState = (SceneE19ActorState *)actor->state;
        if (actorState != 0) {
            *actorState->substate = 4;
        }
        func_80192BFC(selection);
        return;
    case 12:
        return;
    default:
        func_80020CE4();
        func_80192B10(actor, tail);
        func_80192BFC(selection);
        return;
    }
}
