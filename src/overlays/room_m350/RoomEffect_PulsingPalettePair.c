typedef struct { short x, y, z, pad; } Vector;
typedef struct { Vector rotation; Vector *position; int color; } Particle;
typedef struct { short delay, count; } State;
typedef struct { int reserved[2]; void *pool; } Emitter;

/* The spawner owns the particles rendered by this callback. */
extern Emitter *D_800F33E0;
extern int D_800E27EC, D_800F3428, D_8019A414[];
extern unsigned short D_800E1204[], D_800E2850[];
extern short D_800966EE[];
extern int D_800966EC[];
extern Vector D_8019A778[];
extern volatile short D_800F3368, D_800F336A, D_800F3376, D_800F3378;
extern unsigned short D_800F336C;
extern volatile short D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800E11EA, D_800F3370;
extern int GetClut(int, int);
extern int Inv_ScrambleGrid(void);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);

int func_801927A4(int event, Particle *effect) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int scale = D_800966EE[((unsigned int)(D_800E27EC - 1) << 8) & 0x1F00] * 4 + 2048;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;
        if (kind == 4 && D_800F3428) palette += 4;
        handle = GetClut(48, palette);
        func_800CEE20(effect->position, effect, scale, scale, 110,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            &effect->color);
    }
    return 0;
}

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
