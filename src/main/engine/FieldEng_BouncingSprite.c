/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"

int func_800D5010(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, scale, palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 31 / 32;
        state->velocity_z = state->velocity_z * 31 / 32;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        intensity = 128 - (D_800E27EC << 7) / state->duration;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (D_800E27EC << 3);
        scale = rsin((D_800E27EC << 10) / state->duration) + 4096;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(&position, &rotation, scale, scale,
            (s16)D_800F3368.parameter02 * ((D_800E27EC << 3) / state->duration) + 128,
            clut, 1, intensity, 0);
        break;
    }
    return 0;
}
