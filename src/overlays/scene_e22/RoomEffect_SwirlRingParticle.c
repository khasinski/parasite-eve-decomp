#include "pe1/scene_e22_ring_burst.h"
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
                int tpage = D_800E2850[D_800E11FA];
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
