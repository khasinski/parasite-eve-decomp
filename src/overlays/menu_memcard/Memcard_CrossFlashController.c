#include "menu_memcard_glint.h"

/* Controller that draws two crossed beams between the actor's hands while
 * the flash flag is raised, sheds sparks from both hands and mirrors every
 * beam to the other side. */
int Memcard_CrossFlashController(int mode, MemcardCrossFlash *flash) {
    GteShortVector from;
    GteShortVector to;
    MemcardFlashOffset hand = D_801ED818;
    MemcardFlashOffset tipA = D_801ED820;
    MemcardFlashOffset tipB = D_801ED828;
    MemcardFlashOffset center = D_801ED830;
    RenderColor inner = D_801ED838;
    RenderColor outer = D_801ED83C;
    RenderColor edge = D_801ED840;
    RenderMatrixSlot *matrixSlot;
    RoomDampedSpark *child;
    int index;
    int size;
    int alpha;

    switch (mode) {
    case 0:
        flash->attachA = 6;
        flash->attachB = 19;
        flash->reserved04 = 0;
        flash->timer = 0;
        D_801F1F38 = 0;
        tipA.x *= -1;
        tipB.x *= -1;
        func_800CE8F0(D_8009D254, 8, &hand, &from);
        func_800CE8F0(D_8009D254, 8, &tipA, &to);
        func_800D1384(&from, &to, 0x3F0, &inner, &outer, 128, flash->trailA, 1);
        func_800CE8F0(D_8009D254, 8, &tipB, &from);
        func_800D1384(&from, &to, 0x3F2, &edge, &outer, 128, flash->trailC, 1);
        tipA.x *= -1;
        tipB.x *= -1;
        func_800CE8F0(D_8009D254, 21, &hand, &from);
        func_800CE8F0(D_8009D254, 21, &tipA, &to);
        func_800D1384(&from, &to, 0x3F0, &inner, &outer, 128, flash->trailB, 1);
        func_800CE8F0(D_8009D254, 21, &tipB, &from);
        func_800D1384(&from, &to, 0x3F2, &edge, &outer, 128, flash->trailD, 1);
        return func_800CE560(D_800F33E0->pool, 20, 36, Memcard_RisingEmberParticle);
    case 1:
        flash->timer++;
        D_801F1F3A = 1;
        if (D_801F1F38 == 0) {
            int phase = (s16)flash->timer % 3;
            if (phase == 0) {
                func_800CE8F0(D_800F32D0->pool, flash->attachA, &center, &from);
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 1;
                    child->timer = 0;
                }
            } else if (phase == 2) {
                func_800CE8F0(D_800F32D0->pool, flash->attachB, &center, &from);
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (flash->timer & 1) index = 8;
            else index = 21;
            hand.z = -(func_80071A54() & 0x1FF);
            func_800CE8F0(D_800F32D0->pool, index, &hand, &from);
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = from.x;
                child->y = from.y;
                child->z = from.z;
                child->vx = func_80071A54() % 16 - 8;
                child->vy = func_80071A54() % 16 - 8;
                child->vz = func_80071A54() % 16 - 8;
                child->state = 0;
                child->timer = 0;
            }
            if ((flash->timer & 7) == 0) {
                func_800CE870(D_8009D254, 0, (s16 *)&from);
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            break;
        }
        return 2;
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
        alpha = 140;
        size = 0x1000;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        if (D_800E27EC & 1) {
            size = 0x2000;
            alpha = 100;
        }
        {
            int kind;
            int palette;
            func_800CE8F0(D_800F32D0->pool, flash->attachA, &center, &from);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&from, 0, size / 4, size / 4, 128,
                          func_80077AA4(192, palette), 1, alpha / 2, 0);
        }
        {
            int kind;
            int palette;
            func_800CE8F0(D_800F32D0->pool, flash->attachB, &center, &from);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&from, 0, size / 2, size / 2, 128,
                          func_80077AA4(192, palette), 1, alpha / 2, 0);
        }
        tipA.x *= -1;
        tipB.x *= -1;
        func_800CE8F0(D_8009D254, 8, &hand, &from);
        func_800CE8F0(D_8009D254, 8, &tipA, &to);
        func_800D1384(&from, &to, 8, &inner, &outer, 128, flash->trailA, 1);
        func_800CE8F0(D_8009D254, 8, &tipB, &from);
        func_800D1384(&from, &to, 10, &edge, &outer, 128, flash->trailC, 1);
        tipA.x *= -1;
        tipB.x *= -1;
        func_800CE8F0(D_8009D254, 21, &hand, &from);
        func_800CE8F0(D_8009D254, 21, &tipA, &to);
        func_800D1384(&from, &to, 8, &inner, &outer, 128, flash->trailB, 1);
        func_800CE8F0(D_8009D254, 21, &tipB, &from);
        func_800D1384(&from, &to, 10, &edge, &outer, 128, flash->trailD, 1);
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
