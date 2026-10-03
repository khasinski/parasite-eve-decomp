#include "pe1/render_object.h"

typedef struct { short x,y,z,frame; } Particle;
typedef struct { short rotation[3][3]; int position[3]; } Transform;
typedef struct { char reserved[0x238]; Transform *transforms; } Instance;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { signed int unused:16; signed int value:16; } SignedHalf;

/* The spawner owns the falling particles updated and rendered by this callback. */
extern Instance *g_PlayerEntity;
extern Emitter *D_800F33E0;
extern int D_800E27EC;
extern RenderColor D_8019A464;
extern unsigned char D_8019A79A,D_8019A79C;
extern unsigned short D_800E11E4[];
extern short D_800966EE[];
extern int GetClut(int, int);
extern int func_800CE560(void *,int,int,int (*)());
extern Particle *func_800CE610(void *);

int func_801937B4(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3]++;
    } else if (event == 2) {
        int kind = D_800F336C;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 6 : texture + 2);
        func_800CEE20((GteShortVector *)position, 0, 8192, 8192,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            ((SignedHalf *)((char *)D_800966EE - 2 +
              (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)))->value >> 5,
            &D_8019A464);
    }
    return 0;
}

int func_801938E4(int event) {
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,4,func_801937B4);
update:
    if(D_8019A79A) return 2;
    if(D_8019A79C) {
        Particle *particle=func_800CE610(D_800F33E0->pool);
        Instance *player;
        if(!particle) return 0;
        do {
            player=g_PlayerEntity;
            particle->x=player->transforms->position[0];
            particle->y=player->transforms->position[1];
            particle->z=player->transforms->position[2];
            particle->frame=0;
        } while (0);
        D_8019A79C=0;
    }
    goto done;
configure:
    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 0;
done:
    return 0;
}
