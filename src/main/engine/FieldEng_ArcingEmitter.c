#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
int func_800D7B70(int mode, RenderArcingEmitter *state)
{
    RenderColor color;
    GteShortVector position;
    GteRotation rotation;
    RenderArcingEffect *effect;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 8, 24,
                             (FieldAnimTaskCallback)func_800D7A1C);
    case 1:
        if (D_800E27EC < 51) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->x = state->position.x + rcos(state->phase) * state->radius / 4096;
                effect->z = state->position.z + rsin(state->phase) * state->radius / 4096;
                effect->y = state->position.y;
                effect->velocity_y = rand() & 3;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (D_800E27EC >= 74)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 8;
        if (D_800E27EC < 51) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 1024;
            rotation.y = 0;
            rotation.z = D_800E27EC << 5;
            rotation.flags = 1;
            scale = rsin((D_800E27EC << 10) / 50);
            state->radius = scale / 8;
            func_800CF3AC(D_800E18F0, &color, D_800E27EC);
            func_800D0728(&position, 350, 500, 20, &rotation,
                          scale, scale, 0, &color, 128, 1);
        }
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
