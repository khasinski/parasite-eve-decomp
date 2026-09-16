#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
int func_800D7A1C(int mode, RenderArcingEffect *state)
{
    RenderColor color = D_800C22DC;
    GteShortVector position;
    int palette;
    switch (mode) {
    case 1:
        state->y -= state->velocity_y;
        state->velocity_y--;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E18C0, &color, D_800E27EC);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, 0, 4096, 4096, 0x8B,
                     GetClut(0x20, palette), 1, 128, &color);
        break;
    }
    return 0;
}
