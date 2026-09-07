typedef struct { unsigned short value[4]; } Record;
typedef struct { Record records[2]; unsigned char unknown[30]; unsigned char stopped; } State;
typedef struct { int unknown[2]; void *pool; } Context;
extern Context *D_800F33E0;
extern State D_8019AF74;
extern short D_8019AF94,D_8019AF96,D_8019AF98;
extern unsigned short D_800E11FA;
extern unsigned short D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_80198B1C(int,void *);
extern int func_800CE560(void *,int,int,int (*)(int,void *));
extern Record *func_800CE610(void *);

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
