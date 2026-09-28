#include "pe1/room_m350_effects.h"

typedef struct { signed int low:16; signed int high:16; } TrigEntry;
extern int D_800E27EC,D_800F3428;
extern short D_800966EC[],D_800966EE[],D_800F336A;
extern unsigned short D_800F336C,D_800E1204[];
extern int D_8019A3D0[];
extern unsigned short GetClut(int,int);
extern void func_800CEE20(void *,int,int,int,int,int,int,int,void *);
int func_80198860(int event,RoomM350CloudParticle *particle)
{
    if(event==1) {
        int timer=D_800E27EC;
        int shade;
        int size;
        register int product asm("$2");
        if(timer>=8) return 1;
        shade=((TrigEntry *)((char *)D_800966EE-2+(((unsigned int)timer*3<<8)&0x3F00)))->high >> 5;
        particle->state.value.active.secondShade=128-((unsigned int)timer<<4);
        size=particle->size;
        particle->state.value.active.firstShade=shade;
        product=*(short *)((char *)D_800966EC+(((unsigned int)timer<<9)&0x3E00))*size;
        {
            int twice=(unsigned int)product<<1;
            product=(unsigned int)twice+(unsigned int)product;
        }
        particle->state.value.active.animatedSize=product/4096;
    } else if(event==2) {
        GteShortVector world;
        /* Match note: original purpose of unused sp+0x30..0x37 is unknown. */
        char frameGap[8];
        register int kind asm("$2");
        int palette;
        unsigned short clut;
        world.x=(unsigned int)particle->anchor[0]+(unsigned short)particle->position[0];
        world.y=(unsigned int)particle->anchor[1]+(unsigned short)particle->position[1];
        world.z=(unsigned int)particle->anchor[2]+(unsigned short)particle->position[2];
        if(particle->state.value.active.firstShade>0) {
            kind=D_800F336C; palette=D_800E1204[kind];
            if(kind==4 && D_800F3428) palette+=4;
            clut=GetClut(32,palette);
            func_800CEE20(&world,0,particle->size,particle->size,D_800F336A*2+216,clut,1,particle->state.value.active.firstShade,D_8019A3D0);
        }
        kind=D_800F336C; palette=D_800E1204[kind];
        if(kind==4 && D_800F3428) palette+=4;
        clut=GetClut(32,palette);
        func_800CEE20(&world,0,particle->state.value.active.animatedSize,particle->state.value.active.animatedSize,D_800F336A*2+216,clut,1,particle->state.value.active.secondShade,D_8019A3D0);
    }
    return 0;
}


/* MASPSX_FLAGS: --expand-div */
extern RoomM350EffectActor *D_800F32D0;
extern RoomM350EffectEmitter *D_800F33E0;
extern unsigned char D_8019A8BE;
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F336E,D_800F3372,D_800F3374;
extern volatile unsigned short D_800E11E8,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_800CE560(void *,int,int,int (*)(int,RoomM350CloudParticle *));
extern RoomM350CloudParticle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
int func_80198ABC(int event)
{
    GteMatrix matrix;
    GteShortVector vector;
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,20,16,func_80198860);
update:
    {
        int i;
        if(D_8019A8BE) return 2;
        for(i=0;i<2;i++) {
            register int random asm("$16");
            int divisor;
            RoomM350CloudParticle *particle=func_800CE610(D_800F33E0->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            random=Inv_ScrambleGrid();
            random=((unsigned int)random<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=128;
            vector.z=random%divisor;
            ApplyMatrixSV(&matrix,&vector,(GteShortVector *)particle->position);
            particle->position[1]=(unsigned short)particle->position[1]*2+128;
            particle->size=((unsigned int)Inv_ScrambleGrid()<<4)+4096;
            particle->anchor=D_800F32D0->instance->transforms[i ? 11:15].t;
        }
    }
    goto done;
configure:
    {
        int unit=16;
        register int index asm("$4")=D_800E11E8;
        int palette;
        D_800F3372=0;
        D_800F3368=unit; D_800F336A=1;
        D_800F3376=unit; D_800F3378=unit;
        D_800F3376=unit; D_800F3378=unit;
        asm volatile("" : "=r"(index) : "0"(index));
        palette=D_800E2850[index];
        D_800F336C=2; D_800F336E=0;
        D_800F3374=unit; D_800F3370=palette;
    }
done:
    return 0;
}
