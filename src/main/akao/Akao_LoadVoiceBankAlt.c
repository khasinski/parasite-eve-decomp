#include "pe1/scene_assets.h"

void Akao_LoadVoiceBankAlt(void)
{
    Pe1GameState *state = &g_GameState;
    int i, j;
    u32 base;
    SceneBankResetPair *pair;

    for (i = 9; i >= 0; --i)
        state->bank_slots[i] = 0;
    for (i = 0; i < 10; ++i)
        for (j = 47; j >= 0; --j)
            state->bank_rows[i][j] = 0;
    for (i = 0; i < 1; ++i)
        state->bank_reset_940[i] = 0;
    for (i = 0; i < 1; ++i)
        state->bank_reset_944[i] = 0;
    for (i = 0; i < 1; ++i)
        state->bank_reset_948[i] = 0;
    for (i = 0; i < 1; ++i)
        state->bank_reset_94c[i] = 0;
    for (i = 1; i >= 0; --i)
        state->bank_reset_950[i] = 0;
    for (i = 0; i < 1; ++i)
        state->bank_reset_958[i] = 0;

    base = state->voice_bank_base;
    state->bank_work_base = base;
    state->bank_work_end = base + 0x1400;
    state->bank_asset_table =
        Asset_FindTable08ByU32Key(state->bank_asset_source, 0x5EAF6804);
    state->draw_prim_a[8] = 0x27;
    state->draw_prim_a[9] = 13;
    state->bank_state_11 = 0;
    state->bank_state_12 = 0;
    state->flags &= ~0x400000;

    for (i = 0, pair = D_80094488; i < 4; ++i, ++pair) {
        pair->second = 0;
        D_80094488[i].first = 0;
    }
    state->bank_value_f6 = 0x30;
    state->bank_value_f7 = 0x7F;
    state->bank_value_f8 = 0x100;
    state->bank_value_fa = 0x800;
}
