#include "menu_memcard_glint.h"

/* Controller that sits on the scene anchor, plays its sound on frame 3 and
 * spawns spinning halos, rings and model shards on frames 2 and 8. */
int Memcard_SpinBurstController(int mode, MemcardSpinBurst *burst) {
    MemcardSpinParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->x = ((MemcardGlintAnchor *)D_800F32D0->pool)->x;
        burst->y = ((MemcardGlintAnchor *)D_800F32D0->pool)->y;
        burst->z = ((MemcardGlintAnchor *)D_800F32D0->pool)->z;
        func_800CE9D4(D_8009D254, 0, (GteShortVector *)&burst->rotation);
        burst->rotation.x = 0;
        func_800C6D5C(D_800E22D4, 0, 0);
        return func_800CE560(D_800F33E0->pool, 20, 40, Memcard_SpinRingParticle);
    case 1:
        if (D_800E27EC == 3) {
            func_8006DDCC(1201, 0, burst->x, burst->y, burst->z);
        }
        if (D_800E27EC == 2) {
            child = (MemcardSpinParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 6; i++) {
                child = (MemcardSpinParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = (func_80071A54() & 0x7F) - 64;
                    child->y = 0;
                    child->z = (func_80071A54() & 0x1F) + 16;
                    child->rotation.x = burst->rotation.x;
                    child->rotation.y = burst->rotation.y;
                    child->rotation.z = burst->rotation.z;
                    child->rotation.y += 0x400;
                    child->rotation.x += func_80071A54();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 8) {
            child = (MemcardSpinParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.x += 0x400;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 6; i++) {
                child = (MemcardSpinParticle *)func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = 0;
                    child->y = (func_80071A54() & 0x7F) - 64;
                    child->z = (func_80071A54() & 0x1F) + 16;
                    child->rotation.x = burst->rotation.x;
                    child->rotation.y = burst->rotation.y;
                    child->rotation.z = burst->rotation.z;
                    child->rotation.y += func_80071A54();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 2) {
            child = (MemcardSpinParticle *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.y += 0x800;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (D_800E27EC >= 8) {
            return 2;
        }
        break;
    case 2: {
        RenderMatrixSlot *matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        D_801F1F28.x = burst->x;
        D_801F1F28.y = burst->y;
        D_801F1F28.z = burst->z;
        D_800F336C = 1;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    }
    return 0;
}
