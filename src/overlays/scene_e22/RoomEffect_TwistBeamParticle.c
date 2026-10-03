#include "pe1/scene_e22_ring_burst.h"
#include "pe1/gte.h"

/* Twist beam particle: a twisting trail glow (state 0), a flickering beam
 * held until the release flag is set (state 1), or a swelling ring that
 * drifts along its heading (state 2). */
int func_8019702C(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1F4;
    GteShortVector offset;
    RenderColor color;
    RenderColor colorA = D_8018F21C;
    RenderColor colorB = D_8018F220;
    int angle;
    int size;
    int alpha;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.pad += 0x20;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            if (D_80199504 == 0) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.x += 8;
            p->heading.y += 0xC;
            if ((s16)p->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 5;
            func_800CF844(p, &offset, 200, &p->heading,
                          func_80077CF4(angle) * p->radius / 4096, p->heading.pad);
            size = func_80077DC4(angle) * 3 / 2;
            spin.z = p->heading.pad - (s16)p->timer * 30;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            {
                int tpage = D_800E2850[D_800E11FA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CF3AC(D_8019946C, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&offset, &spin, size, size, (s16)D_800F3368.parameter02 + 100,
                          func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                          : palette + 6),
                          1, 0x80, &color);
            return 0;
        }
        case 1: {
            int u;
            int v;
            alpha = 0x96;
            if (p->timer & 1) alpha = 0x80;
            u = ((p->heading.y & 1) << 6) + 0x80;
            v = 0xA0;
            *(u32 *)&color = 0x808080;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            {
                int tpage = D_800E2850[D_800E11EA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            func_800D2370((GteShortVector *)p, (GteRotation *)&p->heading, p->radius,
                          0xBE, u, v, 0x40, 0x20,
                          func_80077AA4(0x20, D_800E120A), &color, 0, alpha, 1);
            return 0;
        }
        case 2:
            size = func_80077DC4((s16)p->timer << 6) / 8;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800D0E88(p, &p->heading, 0x1004, size, &colorA, &colorB,
                          &colorB, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}
