#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"

int func_800D9E5C(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (D_800E27EC < 19)
            state->velocity_y -= 2;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = D_800E27EC << 7;
        rotation.z = 0;
        rotation.flags = 1;
        intensity = rsin((D_800E27EC << 11) / 24) / 32;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, 4096, 4096, 136,
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int func_800D9FD4(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->x);
        return func_800CE560(D_800F33E0->end, 8, 12,
                            (FieldAnimTaskCallback)func_800D9E5C);
    case 1:
        if (D_800E27EC < 32 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + rand() % 400 - 200;
                particle->y = state->y;
                particle->z = state->z + rand() % 400 - 200;
                particle->velocity_y = -(rand() & 7) - 16;
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
