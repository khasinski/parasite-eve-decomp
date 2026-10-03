/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"
#include "pe1/random.h"

int func_800DD9E4(int mode, RenderSettlingSprite *state)
{
    RenderColor color;
    GteShortVector position;
    int scale;
    int blend;
    int palette;

    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2)
            return 1;
        break;
    case 2:
        scale = 0x1000;
        switch (state->stage) {
        case 0:
            blend = 0x1000 - rcos((state->timer << 10) / 28);
            LoadAverageShort12(state, &D_800E223C, 0x1000 - blend, blend,
                               state);
            state->position.y += rsin(state->phase) / 128;
            state->phase += 160;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            func_800CF3AC(D_800E207C, &color, (D_800E27EC << 6) / 28);
            scale = 0x2000 - (state->timer << 12) / 28;
            if (state->timer >= 28) {
                state->stage = 1;
                state->timer = 0;
                state->position.x += (rand() & 255) - 128;
                state->position.y += (rand() & 255) - 128;
                state->position.z += (rand() & 255) - 128;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            blend = rcos((state->timer << 10) / 12) / 32;
            *(u32 *)&color = blend | (blend << 8);
            state->position.x += (rand() & 7) - 3;
            state->position.y += (rand() & 7) - 3;
            state->position.z += (rand() & 7) - 3;
            if (state->timer >= 8)
                state->stage = 2;
            break;
        }
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D1DEC(&position, &color, 128, 1);
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, 0, scale, scale, 128, GetClut(32, palette), 3,
                      128, &color);
        break;
    }
    return 0;
}
