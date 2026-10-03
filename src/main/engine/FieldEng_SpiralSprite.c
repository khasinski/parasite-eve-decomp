#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"

/* One sprite of the spiral: circles the shared anchor D_800E21D8 at radius
 * D_800E21D4 while sinking, and flickers through an eight-frame strip. */
int func_800D629C(int mode, RenderSpiralSprite *state)
{
    RenderColor color = D_800C22C8;
    int scale;
    int intensity;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->position.y = state->height;
        state->position.x = D_800E21D8.x + D_800E21D4 * rcos(state->angle) / 4096;
        state->position.z = D_800E21D8.z + D_800E21D4 * rsin(state->angle) / 4096;
        state->height -= 7;
        state->angle += 2;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        scale = (D_800E27EC << 11) / 24 + 0x800;
        intensity = 128 - (D_800E27EC << 7) / 24;
        state->position.y = D_800E21D8.y - (rand() & 31);
        scale = scale * (((state->phase + D_800E27EC / 2) & 7) * 128 + 0x1000) / 4096;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(&state->position, 0, scale * 2, scale,
                      D_800F336A * ((state->phase + D_800E27EC / 2) & 7) + 128,
                      clut, 1, intensity, &color);
        break;
    }
    return 0;
}
