typedef struct { unsigned short value[4]; } Position;
typedef struct { Position *position; } Effect;
typedef struct { int unknown[2]; void *pool; } Context;
extern Context *D_800F33E0;
extern Position D_8019AF74[2];
extern short D_8019AE88,D_8019AF94,D_8019AF96;
extern int D_800E27EC;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_80199950(int,void *);
extern int func_800CE560(void *,int,int,int (*)(int,void *));
extern Effect *func_800CE610(void *);

int func_80199A90(int mode) {
    switch(mode) {
    case 0:
        D_8019AE88=0;
        return func_800CE560(D_800F33E0->pool,4,6,func_80199950);
    case 1: {
        int i;
        Position *position;
        if(D_8019AE88) return 2;
        if(D_8019AF94==15 && D_8019AF96>=25) {
            D_8019AE88=1;
            return 2;
        }
        if(D_800E27EC&7) return 0;
        i=0;
        position=D_8019AF74;
        for(;i<2;++i) {
            Effect *output=func_800CE610(D_800F33E0->pool);
            if(!output) break;
            output->position=position;
            ++position;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
        unsigned short palette;
        D_800F3368=64;
        D_800F336A=4;
        D_800F3376=64;
        D_800F3378=64;
        D_800F3376=64;
        D_800F3378=64;
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
