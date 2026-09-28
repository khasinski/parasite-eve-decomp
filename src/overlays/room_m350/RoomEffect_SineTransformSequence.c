#include "pe1/room_m350_effects.h"

extern RoomM350EffectActor *D_800F32D0;
extern int D_800E27EC, D_800F3428;
extern unsigned short D_800F336C, D_800E1204[];
extern int D_8019A738[], D_8019A734[];
extern int rsin(int);
extern unsigned short GetClut(int, int);
extern void func_800CEE20(void *, void *, int, int, int, unsigned int, int, int, void *);
extern void func_800D004C(void *, int, int, int, void *, int, int, void *, void *, int, int);

int func_801996BC(int event, RoomM350SineParticle *object)
{
    GteShortVector position;
    GteShortVector texture;
    if (event == 1) {
        int wave = rsin((unsigned int)D_800E27EC << 4);
        object->size = wave * object->amplitude >> 12;
        if (D_800E27EC >= 8) {
            object->brightness -= 8;
            if (object->brightness < 16) return 1;
        }
    } else if (event == 2) {
        int kind = D_800F336C;
        register int special asm("$4") = 4;
        register int index asm("$3") = kind * 2;
        int palette;
        unsigned short handle;
        palette = *(unsigned short *)((char *)D_800E1204 + index);
        if (kind == special && D_800F3428) palette += 7;
        else palette += 3;
        handle = GetClut(0, palette);
        func_800CEE20(object, object->controls, object->size, object->size,
            128, handle, 1, object->brightness, 0);
        position.x = object->position.x;
        position.y = D_800F32D0->instance->transform.t[1];
        position.z = object->position.z;
        texture.x = 1024;
        texture.y = D_800E27EC * 192;
        texture.z = 0;
        texture.pad = 1;
        func_800D004C(&position, 384, 384, 10, &texture, object->size, object->size,
            D_8019A738, D_8019A734, object->brightness, 1);
    }
    return 0;
}


typedef struct { short count, delay; } EmissionState;
extern RoomM350EffectEmitter *D_800F33E0;
extern unsigned char D_8019A8C4;
extern volatile unsigned short D_800E11FA;
extern unsigned short D_800E2850[];
extern volatile short D_800F3368, D_800F336A, D_800F336E, D_800F3372, D_800F3374;
extern volatile unsigned short D_800F3370;
extern volatile short D_800F3376, D_800F3378;
extern int func_800CE560(void *, int, int, int (*)(int, RoomM350SineParticle *));
extern RoomM350SineParticle *func_800CE610(void *);
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
        RoomM350SineParticle *effect;
        int random;
        register int index asm("$3");
        register RoomM350EffectActor *actor asm("$5");
        register RoomM350EffectInstance *instance asm("$6");
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
        effect->position.x = instance->transforms[index].t[0];
        effect->position.y = actor->instance->transforms[index].t[1];
        effect->position.z = actor->instance->transforms[index].t[2];
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
