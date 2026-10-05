/* Sprite callback and poll/reset controller share one pool and layout. */
#include "room_m273_effects.h"
#include "room_m273_sweep.h"
typedef GteShortVector Vector;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);

int func_80197230(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind, palette;
        register int size asm("$18");
        register int sample asm("$2");
        unsigned short clut;
        sample = D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4];
        kind = D_800F336C;
        size = sample + 2048;
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 5, 0);
    }
    return 0;
}

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR D_800F33E0->pool
#define ROOMLIB_POLL_RESET_FUNC func_80197360
#define ROOMLIB_POLL_RESET_CALLBACK func_80197230
#define ROOMLIB_POLL_RESET_FLAG D_8019AF69
#define ROOMLIB_POLL_RESET_COUNTER D_8019AF0A
#define ROOMLIB_POLL_RESET_SEED D_8019AF04
#include "../room_lib/RoomLib_PollAndResetActor.inc"

#include "room_m273_effects.h"
typedef RoomM273RisingParticle Particle;

/* The timed emitter fills the eight-byte records rendered by this callback. */
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern short D_800F336A,D_8019AE84;
extern unsigned short D_800F3368,D_800F336C,D_800F336E,D_800E1204[];
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern unsigned short D_800F3376,D_800F3378;
extern unsigned char D_8019AD70[],D_8019AF6A,D_8019AF69;
extern unsigned short D_800E11E8,D_800E2850[];
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short GetClut(int, int);
extern int func_800CE560(void *,int,int,int (*)());
extern void *func_800CE610(void *);

int func_801974DC(int mode, Particle *input) {
    Particle *particle = input;
    /* Layout only: original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 32) return 1;
        else {
            unsigned short y = ((volatile Particle *)particle)->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            asm("" : "=r"(y) : "0"(y), "r"(speed));
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F336C;
        int sample, palette, size, shade;
        int paletteOffset, sizeOffset;
        unsigned int sampleOffset;
        int clut;
        sampleOffset = ((unsigned int)frame << 8) & 0x3F00;
        asm("" : : "r"(sampleOffset), "r"(frame));
        paletteOffset = kind * 2;
        sample = D_800966EC[sampleOffset / 4];
        asm("" : : "r"(sample), "r"(paletteOffset));
        sizeOffset = ((unsigned int)frame << 7) & 0x3F80;
        palette = *(unsigned short *)((char *)D_800E1204 + paletteOffset);
        shade = (short)sample >> 5;
        size = *(short *)((char *)D_800966EC + sizeOffset) + 8192;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(particle, 0, size, size, D_800F336A * (frame / 4) + 200,
            clut, 1, shade, D_8019AD70);
    }
    return 0;
}

int func_80197648(int mode) {
    switch(mode) {
    case 0:
        D_8019AE84=0;
        return func_800CE560(D_800F33E0->pool,8,12,func_801974DC);
    case 1: {
        Particle *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF6A) return 0;
        if(D_8019AE84-- > 0) return 0;
        output=func_800CE610(D_800F33E0->pool);
        if(!output) return 0;
        ((volatile Particle *)output)->x=D_8019AEFC.x;
        ((volatile Particle *)output)->y=D_8019AEFE-256;
        output->z=D_8019AF00;
        D_8019AE84=2;
        ((volatile Particle *)output)->speed=0;
        break;
    }
    case 2: {
        unsigned int index=D_800E11E8;
        unsigned short palette;
        D_800F3368=16;
        D_800F336A=1;
        D_800F3376=16;
        D_800F3378=16;
        D_800F3376=16;
        D_800F3378=16;
        palette=D_800E2850[index];
        D_800F336C=2;
        D_800F336E=0;
        D_800F3372=0;
        D_800F3374=0;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}


/* The position emitter fills the eight-byte records rendered by this callback. */
extern int D_800E27EC,D_800F3428,D_800966EC[];
extern unsigned short D_800F3368,D_800F336C,D_800F336E,D_800E1204[],D_800942EC;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern unsigned short D_800F3376,D_800F3378;
extern unsigned char D_8019AB70[],D_8019AD74[],D_8019AD78[],D_8019AF68,D_8019AF69;
extern unsigned short D_800E11EA,D_800E2850[];
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short GetClut(int,int);
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);
extern int func_800CE560(void *,int,int,int (*)());

int func_801977F8(int mode,Vector *input) {
    Vector rotation,position;
    if(mode==1) {
        if(D_800E27EC>=16) return 1;
    } else if(mode==2) {
        register int counter asm("$2")=D_800E27EC;
        register int frame asm("$22")=counter-1;
        int kind=D_800F336C;
        register int sample asm("$3")=*(int *)((char *)D_800966EC+(((unsigned int)frame<<8)&0x3F00));
        int shade,size,palette;
        unsigned short clut;
        rotation.x=0; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=0;
        position=*input;
        palette=D_800E1204[kind];
        shade=sample>>21;
        size=(short)sample*3+2048;
        if(kind==4 && D_800F3428) palette+=4;
        clut=GetClut(80,palette);
        func_800CEE20(&position,&rotation,size,size,74,clut,1,shade,0);
        rotation.z>>=1;
        sample=*(int *)((char *)D_800966EC+(((unsigned int)frame<<8)&0x3F00));
        shade=sample>>21;
        size=(short)sample+1024;
        func_800D004C(&position,512,512,16,&rotation,size,size,D_8019AB70,D_8019AD74,shade,1);
        rotation.x=1024; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=1;
        position.y=D_800942EC;
        asm("" : : "r"(frame) : "memory");
        func_800D004C(&position,768,768,8,&rotation,4096,4096,D_8019AD78,D_8019AB70,shade,1);
    }
    return 0;
}

int func_80197A48(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,8,4,func_801977F8);
    case 1: {
        Vector *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF68) return 0;
        output=func_800CE610(D_800F33E0->pool);
        if(!output) return 0;
        ((volatile Vector *)output)->x=D_8019AEFC.x;
        ((volatile Vector *)output)->y=D_8019AEFE;
        output->z=D_8019AF00;
        break;
    }
    case 2: {
        unsigned int index=D_800E11EA;
        unsigned short palette;
        D_800F3368=32;
        D_800F336A=2;
        D_800F3376=32;
        D_800F3378=32;
        D_800F3376=32;
        D_800F3378=32;
        palette=D_800E2850[index];
        asm("" : : "r"(palette) : "memory", "$2");
        D_800F336C=3;
        D_800F336E=0;
        D_800F3372=0;
        D_800F3374=64;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
