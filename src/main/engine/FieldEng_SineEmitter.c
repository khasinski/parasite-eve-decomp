#include "pe1/render_object.h"
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
