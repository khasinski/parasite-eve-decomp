#include "menu_memcard_glint.h"

/* Controller that sits on the scene anchor, bursts fading glints on its
 * early frames and flashes a glint, ring and halo while it fades. */
int Memcard_GlintBurstController(int mode, MemcardGlintBurst *burst) {
    RenderColor color;
    RenderColor flash = D_801ED844;
    RoomDampedSpark *child;
    RenderMatrixSlot *matrixSlot;
    int i;
    int angle;
    int fade;
    int scale;

    switch (mode) {
    case 0:
        burst->x = ((MemcardGlintAnchor *)D_800F32D0->pool)->x;
        burst->y = ((MemcardGlintAnchor *)D_800F32D0->pool)->y;
        burst->z = ((MemcardGlintAnchor *)D_800F32D0->pool)->z;
        return func_800CE560(D_800F33E0->pool, 20, 22, Memcard_FadingGlintParticle);
    case 1:
        if (D_800E27EC == 3) {
            for (i = 0; i < 6; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->vx = func_80071A54() % 50 - 25;
                    child->vy = func_80071A54() % 50 - 25;
                    child->vz = func_80071A54() % 50 - 25;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 5) {
            for (i = 0; i < 4; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->angle = (func_80071A54() & 0x7FF) + 0x800;
                    child->x += (func_80071A54() & 0x7F) - 64;
                    child->y += (func_80071A54() & 0x7F) - 64;
                    child->z += (func_80071A54() & 0x7F) - 64;
                    child->vx = func_80071A54() % 190 - 95;
                    child->vy = func_80071A54() % 190 - 95;
                    child->vz = func_80071A54() % 190 - 95;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC >= 8) {
            return 2;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        if (D_800E27EC == 1) {
            func_800D1AE0(&flash, 128, 2, 8);
        } else if (D_800E27EC == 2) {
            func_800D1AE0(&flash, 128, 1, 8);
        }
        if (D_800E27EC < 33) {
            int kind;
            int palette;
            angle = D_800E27EC << 5;
            fade = rcos(angle) / 32;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (D_800E27EC / 4) + 192,
                          func_80077AA4(192, palette), 1, fade, 0);
            func_800CF3AC(D_801F1CB0, &color, D_800E27EC);
            scale = rsin(angle) + 0x1000;
            if (D_800E27EC & 1) {
                fade = fade * 2 / 3;
            }
            func_800D004C((GteShortVector *)burst, 500, 500, 14, 0, scale, scale,
                          &color, 0, fade, 1);
            fade = rcos(angle) / 32;
            scale = rsin(angle) + 0x800;
            func_800D0728((GteShortVector *)burst, 1100, 1700, 18, 0, scale, scale,
                          0, &color, fade / 2, 1);
        }
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
