typedef struct { short x,y,z,pad; } Vector;
typedef struct { unsigned short value[4]; } Record;
typedef struct { Record records[2]; unsigned char unknown[30]; unsigned char stopped; } State;
typedef struct { int unknown[2]; void *pool; } Context;

/* The emitter owns the eight-byte pool whose entries this callback renders. */

extern int D_800E27EC,D_800F3428;
extern short D_800966EE[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern unsigned short D_800E1204[],D_800E11FA,D_800E2850[],D_800942EC;
extern Vector D_8019AB68;
extern unsigned char D_8019AE18[],D_8019AB70[];
extern Context *D_800F33E0;
extern State D_8019AF74;
extern short D_8019AF94,D_8019AF96,D_8019AF98;
extern unsigned short GetClut(int,int);
extern void func_800CEE20(Vector *,void *,int,int,int,int,int,int,void *);
extern void func_800D004C(Vector *,int,int,int,Vector *,int,int,void *,void *,int,int);
extern int func_800CE560(void *,int,int,int (*)());
extern Record *func_800CE610(void *);

int func_80198B1C(int mode,Vector *input) {
    Vector position;
    if(mode==1) {
        if(D_800E27EC>=32) return 1;
    } else if(mode==2) {
        unsigned int phase=(unsigned int)(D_800E27EC-1)<<5;
        int shadeSample=*(short *)((char *)D_800966EE+(((phase+1024)&4095)*4));
        int sizeSample=*(short *)((char *)D_800966EE+((phase&4095)*4));
        unsigned int shade;
        int size,kind,palette;
        register int specialKind asm("$3");
        unsigned short clut;
        position=*input;
        kind=D_800F336C;
        shade=(unsigned int)(shadeSample+4096)>>5;
        size=sizeSample*2+4096;
        specialKind=4;
        asm("" : : "r"(shade), "r"(size) : "memory");
        palette=D_800E1204[kind];
        if(kind==specialKind && D_800F3428) palette+=8;
        else palette+=4;
        clut=GetClut(0,palette);
        func_800CEE20(&position,0,(short)size,(short)size,100,clut,1,(short)shade,0);
        position.y=D_800942EC;
        func_800D004C(&position,384,384,8,&D_8019AB68,4096,4096,
            D_8019AE18,D_8019AB70,(short)shade,1);
    }
    return 0;
}

int func_80198CD4(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,8,4,func_80198B1C);
    case 1: {
        unsigned char *anchor=(unsigned char *)&D_8019AF74+46;
        Record *source;
        int i;
        if(*anchor) return 2;
        if(D_8019AF94!=15) return 0;
        if(D_8019AF96<4) return 0;
        if(D_8019AF98>=4) break;
        i=0;
        source=(Record *)(anchor-46);
        for(;i<2;++i) {
            Record *output=func_800CE610(D_800F33E0->pool);
            if(!output) break;
            *output=*source;
            ++source;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
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
        D_800F336E=1;
        D_800F3372=0;
        D_800F3374=32;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
