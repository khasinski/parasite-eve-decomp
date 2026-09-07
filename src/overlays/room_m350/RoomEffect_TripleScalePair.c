typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector *position; } Particle;
typedef struct {
    char reserved[14];
    unsigned char animation;
    char reserved0F[7];
    unsigned short frame;
    short reserved18;
    unsigned short previousFrame;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;

/* The controller owns the position pointers rendered by this callback. */
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern int D_800E27EC, D_800F3428, D_800966EC[];
extern unsigned short D_800F336C, D_800E1204[], D_800E2850[];
extern short D_800F336A;
extern Vector D_8019A778[];
extern volatile short D_800F3368,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336E;
extern volatile unsigned short D_800E11FA,D_800F3370;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *,int,int,int (*)());
extern Particle *func_800CE610(void *);

int func_80193A80(int event, void **object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int sample = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00));
        int size;
        int kind;
        int palette;
        int handle;
        register int special asm("$3");

        asm("" : "=r"(sample) : "0"(sample) : "memory");
        kind = D_800F336C;
        asm("" : "=r"(sample), "=r"(kind) : "0"(sample), "1"(kind));
        size = sample * 3;
        asm("" : "=r"(size) : "0"(size), "r"(kind));
        special = 4;
        palette = D_800E1204[kind];
        if (kind == special && D_800F3428) {
            palette += 8;
        } else {
            palette += 4;
        }
        handle = GetClut(0, palette);
        func_800CEE20(*object, 0, size, size, D_800F336A * 6 + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5, 0);
    }
    return 0;
}

int func_80193BCC(int event) {
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,4,2,func_80193A80);
update:
    {
        Instance *instance = D_800F32D0->instance;
        int frame, previous, i;
        register Vector *position asm("$16");
        if (instance->animation != 11) return 0;
        frame = instance->frame;
        previous = instance->previousFrame;
        if (frame >= 5 && previous < 5) {
            position = D_8019A778;
            for (i=0;i<2;i++,position++) {
                Particle *particle = func_800CE610(D_800F33E0->pool);
                if (!particle) break;
                particle->position = position;
            }
            return 0;
        }
        if (frame >= 41) return 2;
    }
    goto done;
configure:
    {
        int index=D_800E11FA;
        int palette;
        D_800F3368=32;
        D_800F336A=2;
        D_800F3376=32;
        D_800F3378=32;
        D_800F3376=32;
        D_800F3378=32;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C=3;
        D_800F336E=1;
        D_800F3372=0;
        D_800F3374=0;
        D_800F3370=palette;
    }
done:
    return 0;
}
