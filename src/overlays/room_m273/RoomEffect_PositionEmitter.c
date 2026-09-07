typedef struct { unsigned short x,y,z,unknown; } Record;
typedef struct { int unknown[2]; void *pool; } Context;
extern Context *D_800F33E0;
extern unsigned char D_8019AF68,D_8019AF69;
extern unsigned short D_8019AEFC;
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short D_800E11EA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_801977F8(int,void *);
extern int func_800CE560(void *,int,int,int (*)(int,void *));
extern Record *func_800CE610(void *);

int func_80197A48(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,8,4,func_801977F8);
    case 1: {
        Record *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF68) return 0;
        output=func_800CE610(D_800F33E0->pool);
        if(!output) return 0;
        ((volatile Record *)output)->x=D_8019AEFC;
        ((volatile Record *)output)->y=D_8019AEFE;
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
