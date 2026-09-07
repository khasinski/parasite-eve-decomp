typedef struct { short x,y,z,pad; } Vector;
typedef struct {
    Vector position;
    short speed, yaw;
    int phase;
    short size, duration, reserved14, frame;
    unsigned char state, reserved19[3];
} Particle;
typedef struct {
    char reserved[14]; unsigned char animation; char reserved0F[7];
    unsigned short frame; short reserved18; unsigned short previousFrame;
    char reserved1C[30]; unsigned short yaw;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern Vector D_8019A7A0;
extern unsigned char D_8019A804;
extern short D_8019A802;
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374,D_800F336C,D_800F336E;
extern volatile unsigned short D_800E11EA,D_800F3370;
extern unsigned short D_800E2850[];
extern int func_801947BC(int, Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
int func_80194CFC(int event) {
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,28,3,func_801947BC);
update:
    {
        Instance *instance;
        Particle *particle;
        int frame, previous;
        if (D_8019A804) return 2;
        instance=D_800F32D0->instance;
        if (instance->animation != 13) return 0;
        frame=instance->frame;
        previous=instance->previousFrame;
        if (frame<15 || previous>=15) return 0;
        particle=func_800CE610(D_800F33E0->pool);
        *(Vector *)particle=D_8019A7A0;
        particle->position.pad=16;
        particle->speed=512;
        {
            int yaw=D_800F32D0->instance->yaw;
            particle->phase=0;
            particle->yaw=yaw;
        }
        asm volatile("" : : : "memory");
        {
            int count=D_8019A802;
            particle->duration=16;
            particle->frame=0;
            *(volatile unsigned char *)&particle->state=0;
            particle->size=100-count*20;
        }
        asm volatile("" : : : "memory");
        D_8019A802++;
    }
    goto done;
configure:
    {
        int index=D_800E11EA;
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
        D_800F336E=0;
        D_800F3372=0;
        D_800F3374=64;
        D_800F3370=palette;
    }
done:
    return 0;
}
