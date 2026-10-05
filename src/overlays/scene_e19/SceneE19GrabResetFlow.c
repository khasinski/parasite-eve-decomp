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

#include "scene_e19_shared.h"

extern volatile int D_800BCF88;
extern u32 SceneE19FlagsWord asm("D_800BCF88");
extern char *D_8009D254;

s32 func_8001AA78();
s32 func_80020CE4();

void func_80192B10(void *arg0, unsigned char *state) {
    {
        SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;

        actor->reset_word = 0;
        actor->reset_halfword = 0;
        actor->flags &= 0xFFFEFFFF;
        actor->h250 &= 0xFBFF;
        func_8001AA78(actor);
    }

    {
        SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;
        int x = actor->pos[0];
        int y = actor->pos[1];
        int z = actor->pos[2];
        int *flags = (int *)&D_800BCF88;

        actor->vel[0] = 0;
        actor->vel[1] = 0;
        actor->vel[2] = 0;
        actor->move[0] = 0;
        actor->move[1] = 0;
        actor->move[2] = 0;
        actor->base_pos[0] = x;
        actor->base_pos[1] = y;
        actor->base_pos[2] = z;
        *flags |= 0x80;
    }

    state[0x38] = 0;
}

typedef struct SceneE19FrameGateLink {
    u8 reserved_00[0x16];
    u16 winLo;
} SceneE19FrameGateLink;

typedef struct SceneE19FrameGateEntity {
    u8 reserved_00[8];
    SceneE19FrameGateLink *link;
} SceneE19FrameGateEntity;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19FrameGateLink, winLo) == 0x16,
                  scene_e19_frame_gate_window_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19FrameGateLink) == 0x18,
                  scene_e19_frame_gate_link_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19FrameGateEntity, link) == 8,
                  scene_e19_frame_gate_entity_link_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19FrameGateEntity) == 0x0C,
                  scene_e19_frame_gate_entity_size);

void RoomLib_RunAfterFrame37_80192BC0(SceneE19FrameGateEntity *o)
{
    if (o->link->winLo >= 0x26) {
        func_80192BFC();
    }
}

#include "scene_e19_shared.h"


int func_80192BFC(SceneE19ResetTarget *ent) {
    unsigned char *state;

    *ent->signal = 0;
    ent->state = 4;
    state = (unsigned char *)ent + 0xC;
    if (ent->enabled != 0) {
        {
            SceneE19ActorChild *child = ((SceneE19Actor *)D_8009D254)->child;

            if (child != 0 && child->h0C > 0) {
                func_80020CE4();
            }
        }

        {
            SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;

            actor->reset_word = 0;
            actor->reset_halfword = 0;
            actor->flags &= 0xFFFEFFFF;
            actor->h250 &= 0xFBFF;
            func_8001AA78(actor);
        }

        {
            volatile SceneE19Actor *actor;
            int x;
            int y;
            int z;

            actor = (SceneE19Actor *)D_8009D254;
            x = actor->pos[0];
            y = actor->pos[1];
            z = actor->pos[2];
            actor->vel[0] = 0;
            actor->vel[1] = 0;
            actor->vel[2] = 0;
            actor->move[0] = 0;
            actor->move[1] = 0;
            actor->move[2] = 0;
            actor->base_pos[0] = x;
            actor->base_pos[1] = y;
            actor->base_pos[2] = z;
            D_800BCF88 |= 0x80;
            state[0x38] = 0;
        }
    }

    return 0;
}


s32 func_80192D04(char *obj)
{
  s32 *out;
  char *sub = obj + 0xC;
  char *e;
  SceneE19ResetTarget *target = (SceneE19ResetTarget *)obj;
  target->state = 4;
  if (target->enabled != 0)
  {
    SceneE19ActorChild *node = ((SceneE19Actor *)D_8009D254)->child;
    if (node != 0)
    {
      if (node->h0C > 0)
      {
        func_80020CE4();
      }
    }
    ;
    ((SceneE19Actor *)D_8009D254)->flags &= 0xFFFEFFFF;
    ((SceneE19Actor *)D_8009D254)->h250 &= 0xFBFF;
    ((SceneE19Actor *)D_8009D254)->reset_word = 0;
    ((SceneE19Actor *)D_8009D254)->reset_halfword = 0;
    func_8001AA78();
    e = D_8009D254;
    *((s32 *) (e + 0x68)) = 0;
    *((s32 *) (e + 0x6C)) = 0;
    *((s32 *) (e + 0x70)) = 0;
    *((s32 *) (e + 0x78)) = 0;
    *((s32 *) (e + 0x7C)) = 0;
    out = (s32 *) (e + 0x40);
    *((s32 *) (e + 0x80)) = 0;
    *out = *((s32 *) (e + 0x28));
    *((s32 *) (e + 0x44)) = *((s32 *) (e + 0x2C));
    *((s32 *) (e + 0x48)) = *((s32 *) (e + 0x30));
    SceneE19FlagsWord |= 0x80;
    sub[0x38] = 0;
  }
  return 0;
}
