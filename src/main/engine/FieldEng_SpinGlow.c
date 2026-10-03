#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_spin_glow.h"

/* Mode 1 moves the glow with damped horizontal speed and gravity for 32
 * ticks; mode 2 draws a pulsing spinning sprite with a fainter halo. */
int func_800DE7A8(int mode, FieldSpinGlow *state)
{
    GteShortVector position;
    GteRotation rotation;
    int scale;
    int intensity;
    int palette;

    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vy = state->vy + 1;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        intensity = state->intensity - state->intensity * D_800E27EC / 32;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + D_800E27EC * 8;
        scale = rsin(D_800E27EC * 32) + 0x2800;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      D_800F336A * (D_800E27EC / 4) + 0xC0, GetClut(64, palette), 1,
                      intensity, 0);
        scale = 0x1800;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      D_800F336A * (D_800E27EC / 8) + 0x60, GetClut(0, palette), 3,
                      intensity, 0);
        break;
    }
    return 0;
}
