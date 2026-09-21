/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"

int func_800D5898(int mode, RenderCosineEffect *state)
{
    RenderColor color = D_800C22C0;

    switch (mode) {
    case 1:
        state->x = state->amplitude
            * rcos((D_800E27EC << 10) / state->duration) / 4096;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        func_800D27FC(state->x, state->y, &color,
            rcos((D_800E27EC << 10) / state->duration) / 64 + 64, 1);
        break;
    }
    return 0;
}
