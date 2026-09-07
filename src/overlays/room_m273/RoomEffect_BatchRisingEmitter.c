typedef struct { unsigned short x,y,z,speed; } Particle;
typedef struct { int unknown[2]; void *pool; } Context;
typedef struct {
    unsigned short x[12],y[12],z[12];
    short count;
    unsigned char unknown[19],stopped;
} Batch;
extern Batch D_8019AF0C;
extern Context *D_800F33E0;
extern unsigned short D_800E11EA,D_800E2850[];
extern unsigned short D_800F3368,D_800F336A,D_800F336C,D_800F336E;
extern unsigned short D_800F3370,D_800F3372,D_800F3374;
extern volatile unsigned short D_800F3376,D_800F3378;
extern int func_80196F2C(int,void *);
extern int func_800CE560(void *,int,int,int (*)(int,void *));
extern Particle *func_800CE610(void *);

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
                Particle *out=func_800CE610(D_800F33E0->pool);
                if(!out) break;
                out->x=*x;
                out->y=*y++-128;
                asm("" : "=r"(i) : "0"(i), "r"(x));
                ++x;
                offset=i*2;
                out->z=*(unsigned short *)((char *)batch+offset+48);
                out->speed=0;
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
