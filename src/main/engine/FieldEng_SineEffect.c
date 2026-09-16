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
