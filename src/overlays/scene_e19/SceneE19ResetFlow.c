#include "scene_e19_shared.h"

extern volatile int D_800BCF88;
extern char *D_8009D254;

void func_8001AA78(SceneE19Actor *actor);

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

typedef struct {
    u8 state;
    char pad1[0xF];
    int *signal;
    char pad14[0x30];
    u8 enabled;
} Entity;


void func_8001AA78(SceneE19Actor *actor);
void func_80020CE4(void);

int func_80192BFC(Entity *ent) {
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
