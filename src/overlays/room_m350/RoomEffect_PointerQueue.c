#include "room_m350_shared.h"
#include "pe1/render_object.h"

typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector *position; short offset,reserved; } Particle;
typedef struct { Vector *positions[4]; char reserved10[0x46]; short count; } Queue;

extern int D_800E27EC, D_800966EC[];
extern RenderColor D_8019A4E8;
extern int GetClut(int, int);
extern RoomM350Emitter *D_800F33E0;
extern short D_8019A7FE, D_8019A802;
extern unsigned char D_8019A804;
extern unsigned short D_800E11E4[];
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);

int func_80194F04(int event, Particle *object)
{
    Vector position;
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        object->offset -= 16;
    } else if (event == 2) {
        int frame,kind,palette,handle;
        position = *object->position;
        position.y += object->offset;
        kind = D_800F336C;
        frame = D_800E27EC - 1;
        palette = D_800E1204[kind];
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2);
        func_800CEE20((GteShortVector *)&position, 0, 8192, 8192, (s16)D_800F3368.parameter02 * (frame >> 1),
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)frame << 7) & 0xF80] >> 5,
            &D_8019A4E8);
    }
    return 0;
}

int func_80195064(int event) {
    char frameGap[8];
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,16,func_80194F04);
update:
    if (D_8019A804 && D_8019A802==0) return 2;
    {
        short *count=&D_8019A7FE;
        if (*count) {
            register int i asm("$16")=0;
            if (*count>0) {
                Queue *queue=(Queue *)((char *)count-(unsigned long)&((Queue *)0)->count);
                Vector **position=queue->positions;
                goto emit;
                for(;i<queue->count;i++,position++) {
emit:
                    {
                        Particle *particle=func_800CE610(D_800F33E0->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->offset=0;
                    }
                }
            }
            D_8019A7FE=0;
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
