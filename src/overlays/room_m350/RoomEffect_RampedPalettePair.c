typedef struct { short x, y, z, pad; } Vector;
typedef struct { Vector *position; short scale, reserved; } Particle;
typedef struct { short target, current; } State;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { char reserved[22]; unsigned short frame; } Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;

/* The spawner owns the scaled particles rendered by this callback. */
extern Emitter *D_800F33E0;
extern Actor *D_800F32D0;
extern int D_800E27EC, D_800F3428, D_800966EC[], D_8019A3D0[];
extern unsigned short D_800F336C, D_800E1204[], D_800E2850[];
extern short D_800F336A;
extern Vector D_8019A778[];
extern volatile short D_800F3368, D_800F3376, D_800F3378;
extern volatile short D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800E11E8, D_800F3370;
extern int GetClut(int, int);
extern void func_800CEE20(void *, int, int, int, int, unsigned int, int, int, void *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);

int func_80192ADC(int event, Particle *object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int size = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)) * object->scale / 4096;
        int kind = D_800F336C;
        int palette = D_800E1204[kind];
        int handle;

        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(object->position, 0, size, size, D_800F336A + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            D_8019A3D0);
    }
    return 0;
}

int func_80192C34(int event, State *state)
{
    if (event == 1) goto update;
    if (event < 2) { if (event == 0) goto setup; goto done; }
    if (event == 2) goto configure;
    goto done;
setup:
    state->target = 16384;
    state->current = 0;
    return func_800CE560(D_800F33E0->pool, 8, 6, func_80192ADC);
update:
    if (!state->target) {
        if (state->current <= 0) return 2;
    } else if (D_800F32D0->instance->frame >= 41) state->target = 0;
    if (D_800E27EC & 7) return 0;
    if (state->current < state->target) state->current += 4096;
    else if (state->target < state->current) state->current -= 4096;
    if (state->current) {
        int i;
        register Vector *position asm("$16") = D_8019A778;
        for (i = 0; i < 2; i++, position++) {
            Particle *particle = func_800CE610(D_800F33E0->pool);
            if (!particle) return 0;
            particle->position = position;
            particle->scale = state->current;
        }
    }
    goto done;
configure:
    {
        int unit = 16;
        int palette;
        asm("" : : "r"(unit) : "memory");
        palette = D_800E2850[D_800E11E8];
        asm("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C = 2; D_800F336E = 0;
        D_800F3368 = unit; D_800F336A = 1;
        D_800F3376 = unit; D_800F3378 = unit;
        D_800F3376 = unit; D_800F3378 = unit;
        D_800F3372 = 0; D_800F3374 = 0; D_800F3370 = palette;
    }
done:
    return 0;
}
