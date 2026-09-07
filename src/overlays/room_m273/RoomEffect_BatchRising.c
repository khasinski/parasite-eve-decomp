typedef struct { short x, y, z, pad; } Vector;
typedef struct { int unknown[2]; void *pool; } Context;
typedef struct {
    unsigned short x[12],y[12],z[12];
    short count;
    unsigned char unknown[19],stopped;
} Batch;

/* The batch emitter fills the eight-byte records rendered by this callback. */
extern int D_800E27EC, D_800F3428;
extern unsigned short D_800E11EA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336C,D_800F336E,D_800E1204[];
extern short D_800F336A;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern short D_8019AD68[];
extern int D_800966EC[];
extern Batch D_8019AF0C;
extern Context *D_800F33E0;
extern unsigned short GetClut(int, int);
extern void func_800CEE20(Vector *, void *, int, int, int, int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)());
extern Vector *func_800CE610(void *);

int func_80196F2C(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        position->y -= 6;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int clut;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(position, 0, 4096, 4096,
            44 + D_800F336A * D_8019AD68[frame / 4], clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4] >> 5, 0);
    }
    return 0;
}

int func_8019706C(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int stack_pad[2];
    switch(mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool,8,40,func_80196F2C);
    case 1: {
        int i;
        unsigned short *x,*y;
        if(D_8019AF0C.stopped) return 2;
        i=0;
        if(D_8019AF0C.count>0) {
            Batch *batch=&D_8019AF0C;
            y=batch->y;
            x=batch->x;
            do {
                int offset;
                Vector *out=func_800CE610(D_800F33E0->pool);
                if(!out) break;
                out->x=*x;
                out->y=*y++-128;
                asm("" : "=r"(i) : "0"(i), "r"(x));
                ++x;
                offset=i*2;
                out->z=*(unsigned short *)((char *)batch+offset+48);
                out->pad=0;
                ++i;
            } while(i<batch->count);
        }
        D_8019AF0C.count=0;
        break;
    }
    case 2: {
        unsigned int index=D_800E11EA;
        unsigned short palette;
        D_800F3368=32; D_800F336A=2;
        D_800F3376=32; D_800F3378=32;
        D_800F3376=32; D_800F3378=32;
        palette=D_800E2850[index];
        D_800F336C=3; D_800F336E=0; D_800F3372=0; D_800F3374=0;
        D_800F3370=palette;
        break;
    }
    }
    return 0;
}
