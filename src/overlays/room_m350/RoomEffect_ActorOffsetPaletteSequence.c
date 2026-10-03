#include "pe1/room_m350_effects.h"


extern RoomM350EffectActor *D_800F32D0;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A56C[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern int GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);

int func_80195B64(int event, GteShortVector *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int entry = D_800966EC[((unsigned int)D_800E27EC << 7) & 0xF80];
        int scale = entry >> 16;
        int shade = (short)entry >> 5;
        int *origin = D_800F32D0->instance->transforms[33].t;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = ((short *)object)[i] * scale / 4096 + origin[i];
        }
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(position, 0, object->pad, object->pad, D_800F336A + 216,
            (unsigned short)handle, 1, shade, D_8019A56C);
    }
    return 0;
}


/* MASPSX_FLAGS: --expand-div */
/* Event state: rotation, emission count, brightness, fade phase and size. */
typedef struct { GteShortVector rotation; short count,brightness,fade,size; } State;
extern RoomM350EffectEmitter *D_800F33E0;
extern unsigned char D_8019A82E;

extern unsigned short D_800E2850[];
extern unsigned short D_800E11E8,D_800F3370;
extern short D_800F3368,D_800F3376,D_800F3378,D_800F336E,D_800F3372,D_800F3374;
extern char D_8019A570[];
extern int Asset_Find08w(int,void *,int,int,int),Inv_ScrambleGrid(void),GetClut(int,int);
extern int func_800CE560(void *,int,int,int (*)(int,GteShortVector *));
extern GteShortVector *func_800CE610(void *);

int func_80195CE0(int event,State *inputState)
{
    State *state=inputState;
    union { GteMatrix matrix; GteShortVector position; } local;
    GteShortVector vector;
    if(event==1) goto update;
    if(event<2) { if(event==0) goto setup; goto done; }
    if(event==2) goto draw;
    goto done;
setup:
    {
        RoomM350EffectInstance *instance=D_800F32D0->instance;
        int *position=instance->transforms[33].t;
        RoomM350EffectEmitter *emitter;
        Asset_Find08w(0x5C9,instance->owner->asset,(short)position[0],(short)position[1],(short)position[2]);
        *(int *)&state->rotation=0;
        emitter=D_800F33E0;
        *(int *)((char *)&state->rotation+4)=0;
        state->count=0; state->fade=0;
        return func_800CE560(emitter->pool,8,18,func_80195B64);
    }
update:
    {
        int phase,sample;
        register int i asm("$19");
        if(D_8019A82E) {
            unsigned short old=*(volatile unsigned short *)&state->fade;
            phase=(short)old*64+1024;
            state->fade=old+1;
            if(phase>=2048) return 2;
        } else {
            int timer=D_800E27EC;
            phase=timer<32 ? (unsigned int)timer<<5:1024;
        }
        sample=(short)D_800966EC[phase&4095];
        state->brightness=sample>>5;
        state->size=(unsigned int)sample<<2;
        if(state->count>=16) goto done;
        i=0;
        do {
            register GteShortVector *particle asm("$17")=func_800CE610(D_800F33E0->pool);
            register int random asm("$16");
            int divisor;
            if(!particle) break;
            i++;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4; vector.z=0;
            RotMatrixYXZ(&vector,&local.matrix);
            asm volatile("" : : "i"(&&radial));
radial:
            vector.x=0; vector.y=0;
            random=Inv_ScrambleGrid();
            random=((unsigned int)random<<8)|Inv_ScrambleGrid();
            divisor=44;
            vector.z=random%divisor+320;
            ApplyMatrixSV(&local.matrix,&vector,particle);
            particle->pad=(unsigned int)state->count*128+2048;
        } while(i<2);
        state->count=(unsigned short)state->count+1;
    }
    goto done;
draw:
    {
        int index=D_800E11E8;
        int i;
        int palette;
        RoomM350EffectActor *actor;
        register int specialKind asm("$17");
        register unsigned short *palettes asm("$19");
        volatile int *position;
        short *out;
        D_800F3368=16; D_800F336A=1;
        D_800F3376=16; D_800F3378=16; D_800F3376=16; D_800F3378=16;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        actor=D_800F32D0;
        D_800F336C=2; D_800F336E=0; D_800F3372=0; D_800F3374=0; D_800F3370=palette;
        asm volatile("" : "=r"(actor) : "0"(actor) : "memory");
        i=0;
        position=actor->instance->transforms[33].t;
        out=&local.position.x;
        for(;i<3;i++) *out++=*position++;
        i=0;
        palettes=D_800E1204;
        state->rotation.z=(unsigned int)D_800E27EC<<8;
        specialKind=4;
        do {
            int kind=D_800F336C;
            int clut,pal=palettes[kind];
            if(kind==specialKind && D_800F3428) pal+=4;
            clut=GetClut(32,pal);
            func_800CEE20(&local.position,state,state->size,state->size,D_800F336A+216,(unsigned short)clut,1,state->brightness,D_8019A570);
            state->rotation.z=0u-(unsigned short)state->rotation.z;
        } while(++i<2);
    }
done:
    return 0;
}
