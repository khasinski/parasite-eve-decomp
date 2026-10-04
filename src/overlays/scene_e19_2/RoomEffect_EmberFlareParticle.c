#include "pe1/scene_e19_2_ember.h"
#include "pe1/gte.h"

/* Ember flare particle: embers drift and bounce off the floor while their
 * sideways speed decays (state 0 slowly, state 1 quickly while falling),
 * and a wide flare pulses in place (state 2). */
int func_801972EC(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1CC;
    GteRotation turn = D_8018F1D4;
    RenderColor color = D_8018F234;
    int fall;
    int bounce;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 7) * 3 / 2;
            glow = func_80077CF4((s16)p->timer << 7) / 160;
            spin.z = -((s16)p->timer * 32);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800CF3AC(D_8019B484, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0x98,
                          func_80077AA4(0x90, palette), 2, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = func_80077DC4(((s16)p->timer << 10) / 24);
            spin.z = (s16)p->timer * 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800CF3AC(D_8019B484, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0x98,
                          func_80077AA4(0x80, palette), 1, 0x80, &color);
            break;
        }
        case 2:
            glow = func_80077DC4(((s16)p->timer << 10) / 24) / 32;
            size = func_80077CF4(((s16)p->timer << 10) / 24);
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_8019B484, &color, (s16)p->timer * 2);
            func_800D0728((GteShortVector *)p, 0x320, 0x578, 0x18, &turn, size, size,
                          0, &color, glow, 1);
            break;
        }
        break;
    }
    return 0;
}
