typedef struct { unsigned short x,y,z,speed; } Record;
typedef struct { int unknown[2]; void *pool; } Context;
extern Context *D_800F33E0;
extern unsigned char D_8019AF6A,D_8019AF69;
extern short D_8019AE84;
extern unsigned short D_8019AEFC;
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short D_800E11E8,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_801974DC(int,void *);
extern int func_800CE560(void *,int,int,int (*)(int,void *));
extern Record *func_800CE610(void *);

int func_80197648(int mode) {
    switch(mode) {
    case 0:
        D_8019AE84=0;
        return func_800CE560(D_800F33E0->pool,8,12,func_801974DC);
    case 1: {
        Record *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF6A) return 0;
        if(D_8019AE84-- > 0) return 0;
        output=func_800CE610(D_800F33E0->pool);
        if(!output) return 0;
        ((volatile Record *)output)->x=D_8019AEFC;
        ((volatile Record *)output)->y=D_8019AEFE-256;
        output->z=D_8019AF00;
        D_8019AE84=2;
        ((volatile Record *)output)->speed=0;
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
