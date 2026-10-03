#include "pe1/room_shake_burst.h"
#include "pe1/gte.h"

/* Sweep particle: while it rides the sweep centre it spins up, pulses its
 * size and drops a decaying copy every other frame; the draw turns and
 * stretches the shared model at its position. */
int func_8019326C(int mode, RoomShakeSweepParticle *p) {
    GteShortVector rotation;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomShakeSweepParticle *child;
    int page;
    int spin;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 1:
        if (p->state == 0) {
            p->x = D_80199904.x;
            p->z = D_80199904.z;
            p->y = D_80199904.y;
            spin = p->angle + 0x20;
            p->angle = spin + D_800E27EC * 4;
            p->size = func_80077CF4((D_800E27EC << 11) / 24) / 32;
            p->stretch = (D_800E27EC << 12) / 24;
            if (D_800E27EC & 1) {
                child = (RoomShakeSweepParticle *)func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->state = 1;
                    child->x = p->x;
                    child->y = p->y;
                    child->z = p->z;
                    child->size = p->size;
                    child->stretch = p->stretch;
                }
            }
            if (D_800E27EC >= 0x18)
                return 1;
        } else {
            p->angle += 3;
            p->size = p->size * 3 / 4;
            if (D_800E27EC >= 4)
                return 1;
        }
        p->timer++;
        break;
    case 2:
        scale.z = 0x400;
        scale.x = 0x400;
        scale.y = p->stretch / 4;
        rotation.x = 0;
        rotation.y = p->angle;
        rotation.z = 0;
        func_80079754(&rotation, &matrix);
        func_80078CC4(&matrix, &scale);
        matrix.t[0] = p->x;
        matrix.t[1] = p->y;
        matrix.t[2] = p->z;
        index = &D_800E11FA;
        tpages = D_800E2850;
        {
            int tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.tpage = tpage;
        }
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        {
            int kind;
            int palette;
            int rawPage = func_80077A64(0, 1, 0, 0);
            page = (u16)(tpages[*index] | rawPage);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            palette = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8 : palette + 4);
            func_800C6EC0(page, (u16)palette);
        }
        func_800C6ED8(1);
        func_800C6EF8(D_800F32D8);
        func_800C7098(D_800F32D8, (u8)(p->size / 3), 0, (u8)(p->size / 2));
        func_800C71E4(D_800F32D8, &matrix);
        func_800C6F4C(D_800F32D8);
        break;
    }
    return 0;
}
