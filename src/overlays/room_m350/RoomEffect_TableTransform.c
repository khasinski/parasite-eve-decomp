#include "room_m350_shared.h"
#include "pe1/render_object.h"

typedef struct { short position[3]; short size; } Particle;
typedef RoomM350TransformMatrix Transform;
typedef struct { char reserved[22]; unsigned short frame; char reserved18[0x220]; Transform *transforms; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;

typedef struct { unsigned char index, alternate; } Entry;
extern Actor *D_800F32D0;
extern Instance *D_8019A7F8;
extern RoomM350Emitter *D_800F33E0;
extern unsigned char D_8019A804;
extern Entry D_8019A4D4[][2];
extern int D_800E27EC, D_800966EC[];
extern unsigned short D_800E11E4[];
extern unsigned char D_8019A4EC[];
extern RenderColor D_8019A3D0;
extern int GetClut(int, int);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);

int func_80195564(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int kind = D_800F336C;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 8 : texture + 4);
        func_800CEE20((GteShortVector *)position, 0, position[3], position[3],
            (s16)D_800F3368.parameter02 * D_8019A4EC[(D_800E27EC - 1) / 2] + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            &D_8019A3D0);
    }
    return 0;
}

int func_8019569C(int event) {
    if (event==1) goto update;
    if (event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,10,func_80195564);
update:
    if(D_8019A804) return 2;
    {
        int frame=D_800F32D0->instance->frame;
        unsigned int relative=frame-7;
        int i;
        if(relative>=6) return 0;
        for(i=0;i<2;i++) {
            Entry *entry=&D_8019A4D4[frame][i];
            Particle *particle;
            Instance *instance;
            int j;
            if(!entry->index) return 0;
            particle=func_800CE610(D_800F33E0->pool);
            if(!particle) return 0;
            if(entry->alternate) instance=D_8019A7F8;
            else instance=D_800F32D0->instance;
            {
                int *source=instance->transforms[entry->index].position;
                for(j=0;j<3;j++) particle->position[j]=source[j];
            }
            particle->size=D_800966EC[(relative<<7)&0xF80]+2048;
        }
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
