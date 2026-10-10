#include "pe1/akao/commands.h"
/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/game_audio_state.h"
#include "pe1/akao/pan3d.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/entity_frame_update.h"
#include "pe1/render_object.h"
#include "pe1/scene_assets.h"

typedef struct AkaoPosPanWork {
    AkaoPackedRect3 rect;
    s32 out1;
    s32 out2;
} AkaoPosPanWork;

extern void *g_LoadedSceneAssetBlock[];
extern void *g_AkaoBgmHandle[];

int Akao_Calc3DPan(AkaoPackedRect3 *pos, int *pan, int *volume);
s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, AkaoPosCoord arg3, AkaoPosCoord arg4, AkaoPosCoord arg5);
int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4);

s32 Asset_Find08w(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    return Akao_SendPositionalCmd(g_LoadedSceneAssetBlock[0], arg0, arg1, arg2, arg3, arg4);
}

s32 Akao_SetPos3D(s32 arg0, s32 arg1, u16 arg2, u16 arg3, u16 arg4) {
    AkaoPosPanWork local;
    s32 ret = 0;

    local.rect.x = arg2;
    local.rect.y = arg3;
    local.rect.z = arg4;
    Akao_Calc3DPan(&local.rect, &local.out1, &local.out2);

    {
        if (D_800B0CE8.reset_pending != 0) {
            u8 **table = &D_800B0CE8.voice_banks[3];

            ret = Akao_Cmd_24(table[arg0], arg1, local.out1, local.out2);
        }
    }

    return ret;
}

s32 Akao_SendPositionalCmdStereo(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    s32 ret;
    void **basep = g_AkaoBgmHandle;

    ret = Akao_SendPositionalCmd(*basep, arg0, arg1, arg2, arg3, arg4);
    Akao_SendPositionalCmd(*basep, arg0 + 1, arg1, arg2, arg3, arg4);
    return ret;
}

s32 Asset_Find08Alt(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    return Akao_SendPositionalCmd(g_AkaoBgmHandle[0], arg0, arg1, arg2, arg3, arg4);
}

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, AkaoPosCoord arg3, AkaoPosCoord arg4, AkaoPosCoord arg5) {
    AkaoPackedRect3 rect;
    s32 out1;
    s32 out2;

    rect.x = arg3;
    rect.y = arg4;
    rect.z = arg5;

    Akao_Calc3DPan(&rect, &out1, &out2);
    return Akao_SendTableCommand(arg0, arg1, arg2, out1, out2);
}

int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4) {
    void *entry = Asset_FindTable2CByU16Key(arg0, arg1);
    int ret;

    if (entry != 0) {
        ret = Akao_Cmd_24(entry, arg2, arg3, arg4);
    } else {
        ret = -1;
    }
    return ret;
}

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
