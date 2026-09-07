typedef struct { unsigned short value[4]; } Position;
typedef struct { Position *position; } Effect;
typedef struct { int unknown[2]; void *pool; } Context;

/* The emitter owns the pointer entries rendered by this callback. */
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E,D_800E1204[];
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern Context *D_800F33E0;
extern Position D_8019AF74[2];
extern short D_8019AE88,D_8019AF94,D_8019AF96;
extern unsigned short D_800E11FA,D_800E2850[];
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)());
extern Effect *func_800CE610(void *);

int func_80199950(int mode, Effect *effect) {
    /* Layout only: the original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int frame = (short)(D_800E27EC - 1);
        int kind = D_800F336C;
        unsigned int sizeOffset, shadeOffset;
        int size, shade, palette;
        register int specialKind asm("$3");
        unsigned short clut;
        sizeOffset = ((unsigned int)frame << 8) & 0x3F00;
        shadeOffset = ((unsigned int)frame << 9) & 0x3E00;
        size = *(int *)((char *)D_800966EC + sizeOffset) + 2048;
        shade = (short)*(int *)((char *)D_800966EC + shadeOffset) >> 6;
        specialKind = 4;
        asm("" : "=r"(size), "=r"(shade) : "0"(size), "1"(shade) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 7;
        else palette += 3;
        clut = GetClut(0, palette);
        func_800CEE20(effect->position, 0, (short)size, (short)size,
            64, clut, 1, (short)shade, 0);
    }
    return 0;
}

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
