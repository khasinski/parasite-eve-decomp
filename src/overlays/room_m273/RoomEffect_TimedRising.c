typedef struct { short x; unsigned short y; short z; unsigned short speed; } Particle;
typedef struct { int unknown[2]; void *pool; } Context;

/* The timed emitter fills the eight-byte records rendered by this callback. */
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern short D_800F336A,D_8019AE84;
extern unsigned short D_800F3368,D_800F336C,D_800F336E,D_800E1204[];
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern unsigned char D_8019AD70[],D_8019AF6A,D_8019AF69;
extern Context *D_800F33E0;
extern unsigned short D_8019AEFC,D_800E11E8,D_800E2850[];
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Particle *, void *, int, int, int, int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)());
extern Particle *func_800CE610(void *);

int func_801974DC(int mode, Particle *input) {
    Particle *particle = input;
    /* Layout only: original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 32) return 1;
        else {
            unsigned short y = ((volatile Particle *)particle)->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((volatile Particle *)particle)->speed;
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
        ((volatile Particle *)output)->x=D_8019AEFC;
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
