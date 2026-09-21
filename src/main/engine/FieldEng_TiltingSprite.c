#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
int func_800DCCCC(int mode, RenderTiltingSprite *state)
{
    RenderColor color;
    int u, v, palette;
    switch (mode) {
    case 1:
        state->rotation.y += state->velocity_y;
        state->rotation.z += 8;
        state->rotation.x = state->initial_tilt - state->initial_tilt * D_800E27EC / 128;
        state->height = 114;
        state->width = rcos(D_800E27EC * 16) / 4 + 200;
        if (D_800E27EC >= 64) return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1F18, &color, D_800E27EC);
        u = ((D_800E27EC / 2) & 3) * 64;
        v = (((D_800E27EC / 2) & 7) / 4) * 16 + 160;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428) palette += 4;
        func_800D2370(&D_800E222C, &state->rotation, state->width, state->height,
                     u, v, 64, 16, GetClut(176, palette), 0, &color, 128, 1);
        break;
    }
    return 0;
}

int func_800DCE94(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    RenderTiltingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 16, 16,
                            (FieldAnimTaskCallback)func_800DCCCC);
    case 1:
        if (D_800E27EC < 17) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->initial_tilt = (rand() & 511) - 256;
                particle->rotation.y = state->phase;
                particle->rotation.z = rand();
                particle->velocity_y = (rand() & 31) - 16;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (D_800E27EC >= 73) return 1;
        break;
    case 2:
        if (D_800E27EC < 65) {
            scale = (D_800E27EC << 6) + 2048;
            D_800F3368.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            func_800CF3AC(D_800E1F18, &color, D_800E27EC);
            draw_mode = 1;
            if (D_800E27EC & 1) draw_mode = 3;
            func_800D0728(&position, 200, 300, 16, 0, scale, scale, &color, 0, 128, draw_mode);
            scale = rsin(D_800E27EC * 16) / 2 + 2048;
            func_800D004C(&position, 700, 700, 16, 0, scale, scale, &color, 0, 128, 1);
            func_800D0728(&position, 600, 800, 24, 0, scale, scale, 0, &color, 128, 3);
        }
        D_800E222C.x = state->position.x;
        D_800E222C.y = state->position.y;
        D_800E222C.z = state->position.z;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}
