#include "menu_memcard_trail.h"

s32 Memcard_SineTrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardLinkOffset offset = D_801ED818;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    s32 scale, kind, palette;

    switch (mode) {
    case 0:
        state->attachment = 25;
        state->phase = 0;
        state->timer = 0;
        return func_800CE560(D_800F33E0->slots, 20, 24, Memcard_SineRotatingImage);
    case 1:
        state->timer++;
        func_800CE8F0(D_8009D254, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = func_800CE610(D_800F33E0->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                func_800CE9D4(D_8009D254, 0, (GteShortVector *)&image->rotation);
                image->phase = 0;
                image->timer = 0;
            }
        }
        if (state->timer >= 32) {
            return 2;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        func_800CE8F0(D_8009D254, state->attachment, &offset, &position);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 32;
        if (state->timer < 33) {
            func_800CF3AC(D_801F1E74, &color, state->timer);
            scale = func_80077CF4(state->timer << 6) / 2 + 2048;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20(&position, 0, scale, scale, 64,
                          func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, 128, &color);
            D_801F1F34.r = color.r;
            D_801F1F34.g = color.g;
            D_801F1F34.b = color.b;
        } else {
            *(u32 *)&D_801F1F34 = 0;
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
