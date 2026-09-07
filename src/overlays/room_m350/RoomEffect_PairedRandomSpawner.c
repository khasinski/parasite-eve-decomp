typedef struct { short x, y, z, pad; } Vector;
typedef struct { Vector rotation; Vector *position; int color; } Particle;
typedef struct { short delay, count; } State;
typedef struct { int reserved[2]; void *pool; } Emitter;
extern Emitter *D_800F33E0;
extern int D_800E27EC, D_8019A414[];
extern Vector D_8019A778[];
extern volatile short D_800F3368, D_800F336A, D_800F3376, D_800F3378;
extern volatile short D_800F336C, D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800E11EA, D_800F3370;
extern unsigned short D_800E2850[];
extern int Inv_ScrambleGrid(void);
extern int func_801927A4(int, Particle *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);

int func_801928D4(int event, State *state)
{
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    state->delay = 0;
    state->count = 0;
    return func_800CE560(D_800F33E0->pool, 16, 18, func_801927A4);
update:
    {
        int i;
        if (D_800E27EC >= 17) return 2;
        for (i = 0; i < 2; i++) {
            Particle *particle = func_800CE610(D_800F33E0->pool);
            if (!particle) break;
            particle->rotation.x = (unsigned int)Inv_ScrambleGrid() << 4;
            particle->rotation.y = (unsigned int)Inv_ScrambleGrid() << 4;
            particle->rotation.pad = 1;
            particle->rotation.z = 0;
            particle->position = &D_8019A778[i];
            particle->color = D_8019A414[(state->count + i) & 7];
        }
        {
            int count = (unsigned short)state->count;
            state->count = count + 1;
            state->delay = 4 - count;
            if (state->delay <= 0) state->delay = 1;
        }
    }
    goto done;
configure:
    {
        int index = D_800E11EA;
        int palette;
        D_800F3368 = 32; D_800F336A = 2;
        D_800F3376 = 32; D_800F3378 = 32;
        D_800F3376 = 32; D_800F3378 = 32;
        palette = D_800E2850[index];
        D_800F336C = 3; D_800F336E = 0;
        D_800F3372 = 0; D_800F3374 = 0; D_800F3370 = palette;
    }
done:
    return 0;
}
