#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"

int func_800D6C58(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22D0;
    int scale, intensity, palette;
    unsigned int frame, angle;
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
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = D_800E27EC;
        intensity = 128;
        angle = state->angle;
        rotation.z = (angle << 8) + (frame << 7);
        palette = D_800E1204[D_800F336C];
        scale = (state->angle & 0x7FF) + 0x400;
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 0xBC,
            GetClut(128, palette), 255, intensity, &color);
        break;
    }
    return 0;
}
