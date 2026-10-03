/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/akao/pan3d.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/render_animation_frame.h"
#include "pe1/render_object.h"

int Akao_Calc3DPan(AkaoPackedRect3 *pos, int *pan, int *volume)
{
    Pe1GameState *state;
    long sxy;
    long x;
    long y;
    int depth;
    int far;
    int span;
    int low;

    Render_SetGteScreenOffset();
    state = &g_GameState;
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ctc2_26(*D_800BCFA8);
    depth = RotTransPers(pos, &sxy, &x, &y);
    x = (short)sxy;
    y = sxy >> 16;
    Render_ResetGteScreenOffset();

    *pan = ((x + 40) << 7) / 400 + 64;
    if ((unsigned int)*pan >= 256) {
        *pan = 255;
    }

    if (depth < g_GameState.bank_value_f8) {
        depth = g_GameState.bank_value_f8;
    } else if (g_GameState.bank_value_fa < depth) {
        depth = g_GameState.bank_value_fa;
    }

    far = state->bank_value_fa;
    depth = far - depth;
    depth = depth * depth / (far - state->bank_value_f8);
    span = state->bank_value_f7;
    low = state->bank_value_f6;
    span -= low;
    *volume = depth * span / (far - state->bank_value_f8) + low;
    if ((unsigned int)*volume >= 128) {
        *volume = 127;
    }
    return 0;
}
