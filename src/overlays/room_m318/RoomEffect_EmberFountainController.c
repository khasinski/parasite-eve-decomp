#include "pe1/room_ember_burst.h"
#include "pe1/gte.h"

/* Ember fountain: follows its actor for 141 frames, swelling a glow; for
 * the first 134 frames it sprays orbiting sparks, embers on odd frames and
 * smoke every fourth. It publishes its position as the fountain centre and
 * draws a halo and ring at it. */
int func_80197CBC(int mode, RoomEmberFountain *fountain) {
    GteShortVector position;
    RenderColor color = D_8018F200;
    RoomOrbitTrailParticle *child;
    int angle;
    int glow;

    switch (mode) {
    case 0:
        fountain->reserved08 = 0;
        fountain->timer = 0;
        fountain->glow = 0;
        fountain->ringScale = 0;
        func_800D3F64(0x5EB, func_800D3FD8());
        func_800D3F64(0x5EC, 0x80);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_80197618);
    case 1:
        fountain->timer++;
        func_800CE870((char *)D_800F32D0->pool, 0, (s16 *)fountain);
        position.x = fountain->x;
        position.y = fountain->y;
        position.z = fountain->z;
        angle = (fountain->timer << 10) / 141;
        fountain->glow = func_80077CF4(angle) / 32;
        fountain->ringScale = func_80077DC4(angle);
        if (fountain->timer < 0x86) {
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (func_80071A54() & 0x3F) - 0x20;
                child->y += (func_80071A54() & 0x3F) - 0x20;
                child->z += (func_80071A54() & 0x3F) - 0x20;
                child->radius = (func_80071A54() & 0x3FF) + 0x3E8;
                child->heading.x = func_80071A54();
                child->heading.y = func_80071A54();
                child->heading.z = func_80071A54();
                child->state = 1;
                child->timer = 0;
            }
            if (fountain->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->y += 0x100 - (func_80071A54() & 0x1FF);
                    child->x += (func_80071A54() & 0x1FF) - 0x100;
                    child->z += (func_80071A54() & 0x1FF) - 0x100;
                    child->heading.x = func_80071A54() % 32 - 0x10;
                    child->heading.y = func_80071A54() % 32 - 0x10;
                    child->heading.z = func_80071A54() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (!(fountain->timer & 3)) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->x += (func_80071A54() & 0x1FF) - 0x100;
                    child->y += (func_80071A54() & 0x1FF) - 0x100;
                    child->z += (func_80071A54() & 0x1FF) - 0x100;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        D_8019993C.x = fountain->x;
        D_8019993C.y = fountain->y;
        D_8019993C.z = fountain->z;
        if (fountain->timer < 0x8D) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        position.x = fountain->x;
        position.y = fountain->y;
        position.z = fountain->z;
        if (fountain->timer < 0x8E) {
            glow = fountain->glow;
            if (D_800E27EC & 1)
                glow = glow * 3 / 4;
            D_800F3368.depth = 0x28;
            func_800D004C(&position, 1000, 1000, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            func_800D0728(&position, 0x578, 0x708, 0x18, 0, fountain->ringScale,
                          fountain->ringScale, 0, &color, glow / 2, 1);
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
