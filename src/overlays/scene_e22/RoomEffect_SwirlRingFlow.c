#include "pe1/scene_e22_ember_burst.h"
#include "pe1/gte.h"

/* Swirl ring particle: a ring that grows and fades for 12 frames while its
 * heading turns (state 0), or a glow that swings out along its turning
 * heading for 24 frames (state 1). */
int func_801957CC(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 8;
            p->heading.y += 0xC;
            if ((s16)p->timer < 0xC) break;
            return 1;
        case 1:
            p->timer++;
            p->heading.x += 0x1D;
            p->heading.y += 0xB;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = func_80077DC4(((s16)p->timer << 10) / 12) / 8;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80199434, &color, (s16)p->timer);
            func_800D0E88(p, &p->heading, 0x7D0, size, &color, 0, 0, 0x80, 1);
            return 0;
        case 1:
            angle = ((s16)p->timer << 10) / 24;
            func_800CFB7C(&p->heading, (s16)(func_80077DC4(angle) * p->radius / 4096),
                          &offset);
            offset.x += p->x;
            offset.y += p->y;
            offset.z += p->z;
            size = func_80077DC4(angle) * 2;
            spin.z = (s16)p->timer * 32;
            glow = func_80077CF4(angle) / 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&offset, &spin, size, size, (s16)D_800F3368.parameter02 + 100,
                              func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                              : palette + 4),
                              1, glow, 0);
            }
            break;
        }
        break;
    }
    return 0;
}

/* Swirl ring controller: rides an actor joint and swells a glow for 30
 * frames, spraying four swirl rings per frame for the first eight, then
 * bursts eight swirling glows and fades for 20 frames. It draws a halo
 * and a spinning flare while it swells, a flashing ring while it fades. */
int func_80195B40(int mode, SceneE22SwirlRing *swirl) {
    GteShortVector offset = D_8018F1E0;
    GteShortVector position;
    GteRotation spin = D_8018F1F4;
    RenderColor color = D_8018F214;
    RoomOrbitTrailParticle *child;
    void **soundSlot;
    int volume;
    int angle;
    int glow;
    int i;

    switch (mode) {
    case 0:
        swirl->state = 0;
        swirl->timer = 0;
        swirl->glow = 0;
        swirl->ring = 0;
        soundSlot = &D_800B0E64.channel;
        if (*soundSlot != 0) {
            volume = 0x7F;
            func_8006DF50(*soundSlot, 0x5D6, func_800D3FD8(), 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5D7, 0x80, 0x80, volume);
        }
        func_800CE8F0(D_800F32D0->pool, 0, &offset, swirl);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x28, func_801957CC);
    case 1:
        position.x = swirl->x;
        position.y = swirl->y;
        position.z = swirl->z;
        switch (swirl->state) {
        case 0:
            swirl->timer++;
            angle = (swirl->timer << 10) / 30;
            swirl->glow = func_80077CF4(angle) / 32;
            swirl->ring = func_80077DC4(angle) + 0x400;
            if (swirl->timer < 9) {
                for (i = 0; i < 4; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = position.x;
                        child->y = position.y;
                        child->z = position.z;
                        child->x += (func_80071A54() & 0xFF) - 0x80;
                        child->y += (func_80071A54() & 0xFF) - 0x80;
                        child->z += (func_80071A54() & 0xFF) - 0x80;
                        child->radius = (func_80071A54() & 0x3FF) + 0x578;
                        child->heading.x = func_80071A54();
                        child->heading.y = func_80071A54();
                        child->heading.z = func_80071A54();
                        child->state = 1;
                        child->timer = 0;
                    }
                }
            }
            if (swirl->timer < 0x1E) break;
            swirl->timer = 0;
            swirl->state = 1;
            for (i = 0; i < 8; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->state = 0;
                    child->timer = 0;
                }
            }
            break;
        case 1:
            swirl->timer++;
            angle = (swirl->timer << 10) / 20;
            swirl->glow = func_80077DC4(angle) / 32;
            swirl->ring = func_80077CF4(angle);
            if (swirl->timer < 0x14) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        position.x = swirl->x;
        position.y = swirl->y;
        position.z = swirl->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11FA.index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        switch (swirl->state) {
        case 0: {
            int kind;
            int palette;
            glow = swirl->glow;
            if (D_800E27EC & 1)
                glow = glow * 3 / 4;
            D_800F3368.depth = 0x20;
            func_800D004C(&position, 800, 700, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            spin.z = swirl->timer * 16;
            D_800F3368.parameter02 = 2;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA.index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)swirl, &spin, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * (swirl->timer & 1) + 0x64,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4),
                          1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            int size;
            int timer = swirl->timer;
            if (timer == 1)
                func_800D1AE0(&color, 0x80, 2, 8);
            else if (timer < 6)
                func_800D1AE0(&color, func_80077CF4(((timer - 2) << 10) / 3) / 32, 1, 8);
            glow = swirl->glow;
            D_800F3368.depth = 0x20;
            func_800D0728(&position, 0x44C, 0x5DC, 0x18, 0, swirl->ring, swirl->ring, 0,
                          &color, glow / 2, 1);
            size = swirl->ring + 0x1000;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&position, 0, size, size, 0x44,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                         : palette + 6),
                          1, glow, 0);
            break;
        }
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}
