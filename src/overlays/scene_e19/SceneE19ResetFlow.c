#include "scene_e19_shared.h"

extern volatile int D_800BCF88;
extern u32 SceneE19FlagsWord asm("D_800BCF88");
extern char *D_8009D254;

s32 func_8001AA78();
s32 func_80020CE4();

void func_80192B10(void *arg0, unsigned char *state) {
    {
        SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;

        actor->field_1D8 = 0;
        actor->field_1DC = 0;
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

#include "../room_lib/room_lib.h"

void RoomLib_RunAfterFrame37_80192BC0(RoomEnt *o)
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

            actor->field_1D8 = 0;
            actor->field_1DC = 0;
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
    *((s32 *) (D_8009D254 + 0x98)) &= 0xFFFEFFFF;
    *((u16 *) (D_8009D254 + 0x250)) &= 0xFBFF;
    *((s32 *) (D_8009D254 + 0x1D8)) = 0;
    *((s16 *) (D_8009D254 + 0x1DC)) = 0;
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
