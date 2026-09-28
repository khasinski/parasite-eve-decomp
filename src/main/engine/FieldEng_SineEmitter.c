#include "pe1/render_object.h"
#include "pe1/gte.h"

int func_800DAF8C(int mode, RenderSineEffect *state)
{
    RenderColor color;
    switch (mode) {
    case 1:
        state->position.y += state->velocity_y;
        state->position.z += 8;
        state->position.x = state->amplitude - state->amplitude * D_800E27EC / 64;
        state->angle = rsin(D_800E27EC << 5) / 64;
        state->scale = 1600 - D_800E27EC * 1000 / 64;
        if (D_800E27EC >= 64)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1C2C, &color, D_800E27EC);
        func_800D0E88(&D_800E221C, &state->position, state->scale, state->angle,
                     &color, 0, 0, 128, 1);
        break;
    }
    return 0;
}

#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/battle_runtime.h"

int func_800DB0D0(int mode, RenderSineEmitter *state)
{
    RenderSineEffect *effect;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 16, 16,
                             (FieldAnimTaskCallback)func_800DAF8C);
    case 1:
        if (D_800E27EC < 17) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->amplitude = (rand() & 1023) - 512;
                effect->position.y = state->phase;
                effect->position.z = rand();
                effect->velocity_y = (rand() & 127) - 64;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (D_800E27EC >= 73)
            return 1;
        break;
    case 2:
        D_800E221C.x = state->position.x;
        D_800E221C.y = state->position.y;
        D_800E221C.z = state->position.z;
        D_800F3374 = 8;
        break;
    }
    return 0;
}
