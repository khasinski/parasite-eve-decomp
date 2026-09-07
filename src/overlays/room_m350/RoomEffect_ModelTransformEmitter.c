typedef struct { short x, y, z, pad; } Vector;
typedef struct { Vector position; short controls[4]; short brightness, size, amplitude, reserved; } Particle;
typedef struct { short count, delay; } EmissionState;
typedef struct { int reserved[2]; void *pool; } Emitter;
typedef struct { unsigned char reserved[2], transformCount; } Model;
typedef struct { short rotation[3][3]; int position[3]; } Transform;
typedef struct {
    char reserved[0x1B4];
    Model *model;
    char reserved1B8[0x80];
    Transform *transforms;
} Instance;
typedef struct { int reserved[2]; Instance *instance; } Actor;
extern Emitter *D_800F33E0;
extern Actor *D_800F32D0;
extern unsigned char D_8019A8C4;
extern volatile unsigned short D_800E11FA;
extern unsigned short D_800E2850[];
extern volatile short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800F336C, D_800F3370;
extern volatile short D_800F3376, D_800F3378;
extern int func_801996BC(int, Particle *);
extern int func_800CE560(void *, int, int, int (*)(int, Particle *));
extern Particle *func_800CE610(void *);
extern int Inv_ScrambleGrid(void);
extern int rand(void);

int func_8019988C(int event, EmissionState *state)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        goto done;
    }
    if (event == 2) goto configure;
    goto done;
setup:
    state->count = 0;
    state->delay = 0;
    return func_800CE560(D_800F33E0->pool, 24, 8, func_801996BC);
update:
    if (state->count < 24) {
        Particle *effect;
        int random;
        register int index asm("$3");
        register Actor *actor asm("$5");
        register Instance *instance asm("$6");
        state->delay--;
        if (state->delay != -1) return 0;
        effect = func_800CE610(D_800F33E0->pool);
        if (!effect) return 0;
        random = Inv_ScrambleGrid();
        actor = D_800F32D0;
        instance = actor->instance;
        {
            register int byte asm("$4") = (unsigned char)random;
            int count = instance->model->transformCount;
            register int range asm("$3") = count - 1;
            register int product asm("$2");
            asm("" : "=r"(range) : "0"(range), "r"(count));
            product = byte * range;
            product /= 256;
            index = product + 1;
        }
        asm("" : "=r"(index) : "0"(index));
        effect->position.x = instance->transforms[index].position[0];
        effect->position.y = actor->instance->transforms[index].position[1];
        effect->position.z = actor->instance->transforms[index].position[2];
        {
            int count = state->count;
            effect->brightness = 128;
            effect->amplitude = count * 2048 + 2048;
        }
        /* Preserve the random value until the position/amplitude stores finish. */
        asm("" : "=r"(random) : "0"(random) : "memory");
        {
            int delay = random & 7;
            unsigned short count = *(volatile unsigned short *)&state->count;
            delay += 12;
            state->delay = delay;
            state->count = count + 1;
        }
        {
            register int random asm("$4") = rand();
            int high;
            int y;
            int rawHigh;
            int bit;
            effect->controls[0] = (random & 255) + 512;
            asm volatile("" : : : "memory");
            rawHigh = random >> 8;
            high = rawHigh & 255;
            asm("" : "=r"(high) : "0"(high), "r"(rawHigh), "r"(random));
            bit = random & 2;
            asm("" : "=r"(bit) : "0"(bit), "r"(random));
            if (bit) y = high + 768;
            else y = high - 1024;
            effect->controls[1] = y;
        }
        effect->controls[2] = 0;
        effect->controls[3] = 0;
    } else {
        D_8019A8C4 = 1;
        return 2;
    }
    goto done;
configure:
    {
        int unit = 64;
        int index = D_800E11FA;
        int palette;
        D_800F3368 = unit;
        D_800F336A = 4;
        D_800F3376 = unit;
        D_800F3378 = unit;
        palette = D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        D_800F336C = 3;
        D_800F336E = 1;
        D_800F3372 = 5;
        D_800F3374 = 0;
        D_800F3370 = palette;
    }
done:
    return 0;
}
