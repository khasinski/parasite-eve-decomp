#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
int func_800DC5BC(int mode, GteShortVector *state)
{
    RenderColor color;
    GteRotation rotation;
    int scale, palette;
    int selector;
    register int special asm("$3");
    switch (mode) {
    case 1:
        state->x += (rand() & 7) - 3;
        state->z += (rand() & 7) - 3;
        state->y -= (rand() & 3) + 2;
        if (D_800E27EC >= 14) return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        func_800CF3AC(D_800E1E64, &color, D_800E27EC);
        scale = rsin((D_800E27EC << 11) / 14);
        asm volatile("" : : "r"(scale));
        selector = D_800F336C;
        special = 4;
        palette = D_800E1204[selector];
        if (selector == special && D_800F3428) palette += 6;
        else palette += 2;
        func_800CEE20(state, &rotation, scale, scale,
                     226,
                     GetClut(0, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800DC750(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, &state->x);
        return func_800CE560(D_800F33E0->end, 8, 20,
                            (FieldAnimTaskCallback)func_800DC5BC);
    case 1:
        if (D_800E27EC < 40) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + (rand() & 511) - 256;
                particle->y = state->y + (rand() & 511) - 256;
                particle->z = state->z + (rand() & 511) - 256;
            }
        }
        if (D_800E27EC >= 53) return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
