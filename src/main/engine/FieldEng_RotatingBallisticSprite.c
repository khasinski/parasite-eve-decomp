#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"

int func_800DA780(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (D_800E27EC < 19)
            state->velocity_y -= 1;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->y + state->x + (D_800E27EC << 5);
        rotation.flags = 0;
        intensity = rsin((D_800E27EC << 11) / 24) / 128;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, 4096, 4096,
                     96 + (s16)D_800F3368.parameter02 * (D_800E27EC / 6),
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}
