#include "menu_memcard_glint.h"

/* Ring burst: sprays drift glows on its first frames, then grows a stack
 * of glow rings and bands around its position. */
int Memcard_RingBurstController(int mode, MemcardRingBurst *burst) {
    GteShortVector position;
    GteShortVector offset;
    GteRotation spin = D_801ED7FC;
    GteRotation tilt = D_801ED804;
    RenderColor glow = D_801ED80C;
    RenderColor ring = D_801ED810;
    RenderColor band = D_801ED814;
    RoomDampedSpark *child;
    int i;
    int angle;
    int fade;
    int scale;
    int amount;
    int dim;

    switch (mode) {
    case 0:
        func_800CE870(D_8009D254, 0, &burst->x);
        func_8006DDCC(0x4B7, 0, burst->x, burst->y, burst->z);
        burst->state = 0;
        burst->timer = 0;
        return func_800CE560(D_800F33E0->pool, 20, 50, Memcard_DriftGlowParticle);
    case 1:
        switch (burst->state) {
        case 0:
            if (++burst->timer < 24) {
                return 0;
            }
            burst->state = 1;
            burst->timer = 0;
            for (i = 0; i < 50; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->vx = func_80071A54() % 140 - 70;
                    child->vy = func_80071A54() % 140 - 70;
                    child->vz = func_80071A54() % 140 - 70;
                    child->state = (i & 3) == 0;
                    child->timer = 0;
                }
            }
            return 0;
        case 1:
            if (++burst->timer >= 48) {
                return 1;
            }
            break;
        }
        break;
    case 2:
        position.x = burst->x;
        position.y = burst->y;
        position.z = burst->z;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 41;
        switch (burst->state) {
        case 0:
            angle = (burst->timer << 10) / 24;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            fade = func_80077CF4(angle) / 32;
            if (D_800E27EC & 1) {
                fade = fade * 15 / 16;
            }
            spin.z = burst->timer * 15;
            func_800D004C(&position, 300, 500, 70, &spin, 0x1000, 0x1000,
                          &glow, 0, fade / 2, 1);
            fade = func_80077CF4(angle) / 32;
            func_800D004C(&position, 700, 700, 20, 0, 0x1000, 0x1000,
                          &ring, 0, fade, 1);
            break;
        case 1:
            angle = (burst->timer << 10) / 48;
            if (burst->timer < 5) {
                amount = 0x80 - burst->timer * 32;
                func_800D1AE0(&ring, amount, 1, 8);
            }
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            fade = func_80077DC4(angle) / 32;
            if (D_800E27EC & 1) {
                fade = fade * 15 / 16;
            }
            spin.z = func_80077CF4(angle) / 2;
            func_800D004C(&position, 70, func_80077DC4(angle) / 8 + 700, 8,
                          &spin, 0x1000, 0x1000, &ring, 0, fade / 2, 1);
            func_800D004C(&position, 300, 300, 8, 0, 0x1000, 0x1000,
                          &glow, 0, fade, 1);
            fade = func_80077DC4(angle) / 32;
            func_800D004C(&position, 1000, 1000, 16, 0, 0x1000, 0x1000,
                          &ring, 0, fade, 1);
            scale = func_80077CF4(angle) / 4 + 0xC00;
            func_800D0728(&position, 1500, 1900, 24, 0, scale, scale,
                          0, &ring, fade, 1);
            scale = func_80077CF4(angle);
            func_800D0728(&position, 2000, 2600, 20, &tilt, scale, scale,
                          &band, 0, fade, 1);
            amount = func_80077CF4(angle) / 12 + 80;
            dim = fade * 2 / 3;
            offset.x = position.x;
            offset.y = position.y;
            offset.z = position.z;
            offset.y -= amount;
            func_800D0728(&offset, 1000, 1600, 16, &tilt, scale, scale,
                          &band, 0, dim, 1);
            offset.x = position.x;
            offset.y = position.y;
            offset.z = position.z;
            offset.y += amount;
            func_800D0728(&offset, 1000, 1600, 16, &tilt, scale, scale,
                          &band, 0, dim, 1);
            break;
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        break;
    }
    return 0;
}
