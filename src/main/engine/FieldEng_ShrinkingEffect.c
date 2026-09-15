#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
int func_800D7E78(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    int scale, clut_y;
    switch (mode) {
    case 1:
        state->y--;
        if (D_800E27EC >= 6)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        func_800CF3AC(D_800E1988, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 6;
        clut_y = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            clut_y += 4;
        func_800CEE20(&position, 0, scale, scale, 0x8A,
                     GetClut(0x70, clut_y), 1, 128, &color);
        break;
    }
    return 0;
}
