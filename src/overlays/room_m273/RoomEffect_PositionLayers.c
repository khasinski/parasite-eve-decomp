typedef struct { short x,y,z,pad; } Vector;
typedef struct { int unknown[2]; void *pool; } Context;

/* The position emitter fills the eight-byte records rendered by this callback. */
extern int D_800E27EC,D_800F3428,D_800966EC[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E,D_800E1204[],D_800942EC;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern unsigned char D_8019AB70[],D_8019AD74[],D_8019AD78[],D_8019AF68,D_8019AF69;
extern Context *D_800F33E0;
extern unsigned short D_8019AEFC,D_800E11EA,D_800E2850[];
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short GetClut(int,int);
extern void func_800CEE20(Vector *,Vector *,int,int,int,int,int,int,void *);
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);
extern int func_800CE560(void *,int,int,int (*)());
extern Vector *func_800CE610(void *);

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
        ((volatile Vector *)output)->x=D_8019AEFC;
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
