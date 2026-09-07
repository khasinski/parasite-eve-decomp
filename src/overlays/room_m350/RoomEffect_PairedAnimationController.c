typedef struct { short x,y,z,pad; } Vector;
typedef struct { Vector *position; short frame, reserved; } Particle;
typedef struct { int reserved[2]; int soundMode; } Owner;
typedef struct { short rotation[3][3]; int position[3]; } Transform;
typedef struct {
    Owner *owner;
    char reserved04[10];
    unsigned char animation;
    char reserved0F[7];
    unsigned short frame;
    short reserved18;
    unsigned short previousFrame;
    char reserved1C[0x21C];
    Transform *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern Vector D_8019A778[];
extern volatile short D_800F3368,D_800F336A,D_800F3376,D_800F3378,D_800F3372,D_800F3374;
extern int func_80192E4C(int, Particle *);
extern int func_800CE560(void *,int,int,int (*)(int,Particle *));
extern Particle *func_800CE610(void *);
extern int Asset_Find08w(int,int,int,int,int);
int func_8019360C(int event) {
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool,8,2,func_80192E4C);
update:
    {
        Instance *instance = D_800F32D0->instance;
        int frame;
        int previous;
        int i;
        register Vector *position asm("$16");
        if (instance->animation != 11) return 0;
        frame = instance->frame;
        if (frame >= 50) return 2;
        previous = instance->previousFrame;
        if (frame < 8) return 0;
        if (previous < 8) {
            position = D_8019A778;
            for (i=0;i<2;i++,position++) {
                Particle *particle = func_800CE610(D_800F33E0->pool);
                if (!particle) break;
                particle->position = position;
                particle->frame = 0;
            }
            {
                Instance *current = D_800F32D0->instance;
                Asset_Find08w(0x5C6,current->owner->soundMode,
                    *(short *)&current->transforms->position[0],
                    *(short *)&current->transforms->position[1],
                    *(short *)&current->transforms->position[2]);
            }
        }
    }
    goto done;
configure:
    D_800F3368=32;
    D_800F336A=2;
    D_800F3376=32;
    D_800F3378=32;
    D_800F3376=32;
    D_800F3378=32;
    D_800F3372=0;
    D_800F3374=0;
done:
    return 0;
}
