typedef struct { short rotation[3][3]; int position[3]; } Matrix;
typedef struct { char reserved[0x238]; Matrix *transforms; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { short x, y, z, velocityY, gravity, reserved; } Particle;
extern Actor *D_800F32D0;
extern Emitter *D_800F33E0;
extern Instance *g_PlayerEntity;
extern unsigned char D_8019A82F;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A598[];
extern unsigned short D_800F336C, D_800E1204[];
extern short D_800F336A;
extern volatile short D_800F3368, D_800F3376, D_800F3378;
extern volatile short D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800E11E8, D_800F3370;
extern unsigned short D_800E2850[];
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);

int func_801968D8(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] += position[3];
        position[3] -= position[4];
    } else if (event == 2) {
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;
        if (kind == 4 && D_800F3428) palette += 4;
        handle = GetClut(16, palette);
        func_800CEE20(position, 0, 12288, 12288,
            D_800F336A * (((D_800E27EC - 1) >> 1) & 7) + 200,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A598);
    }
    return 0;
}

int func_80196A14(int event)
{
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    return func_800CE560(D_800F33E0->pool, 12, 16, func_801968D8);
update:
    if (D_800E27EC >= 107) return 2;
    if (D_800E27EC & 1) {
        Matrix *matrices = D_800F32D0->instance->transforms;
        Particle *particle = func_800CE610(D_800F33E0->pool);
        if (particle) {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[33].position[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[33].position[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[33].position[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((D_800E27EC >> 1) & 1) + 2;
        }
    }
    if (D_8019A82F) {
        Matrix *matrices = g_PlayerEntity->transforms;
        Particle *particle = func_800CE610(D_800F33E0->pool);
        if (!particle) return 0;
        {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[0].position[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[0].position[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[0].position[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((D_800E27EC >> 1) & 1) + 2;
        }
    }
    goto done;
configure:
    {
        int index = D_800E11E8;
        int palette;
        D_800F3368 = 16; D_800F336A = 1;
        D_800F3376 = 16; D_800F3378 = 16;
        D_800F3376 = 16; D_800F3378 = 16;
        palette = D_800E2850[index];
        D_800F336C = 2; D_800F336E = 0;
        D_800F3372 = 0; D_800F3374 = 0; D_800F3370 = palette;
    }
done:
    return 0;
}
