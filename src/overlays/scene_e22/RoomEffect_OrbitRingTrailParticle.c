#include "pe1/scene_e22_ring_burst.h"
#include "pe1/gte.h"

/* Ring burst particle: sweeps out from the orbit centre drawing a coloured
 * trail, flashes as a spinning sprite, or drifts as a widening ring; the
 * update side drifts and bounces the flashing ones on the floor. */
int func_80192548(int mode, RoomOrbitTrailParticle *p) {
    GteShortVector offset;
    GteRotation spin;
    RenderColor color;
    RenderColor seed;
    int fall;
    int bounce;
    int scale;
    int size;
    int glow;
    int radius;

    seed = D_8018F1CC;
    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 4;
            p->heading.y += 0x18;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.x += 2;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            scale = (func_80077CF4(((s16)p->timer << 10) / 24) + 0x1000) / 2;
            func_800CFB7C(&p->heading, (s16)(p->radius * scale / 4096), &offset);
            offset.x += D_801994EC.x;
            offset.y += D_801994EC.y;
            offset.z += D_801994EC.z;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_80199168, &color, (s16)p->timer);
            func_800D2B58(&D_801994EC, &offset, &color, 0, 0x80, 0, 1);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;
            size = func_80077CF4((s16)p->timer << 6) + 0x800;
            glow = func_80077DC4((s16)p->timer * 1204 / 16) / 128 + 0x28;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)p, &spin, size, size,
                          (s16)D_800F3368.parameter02 * ((s16)p->timer / 2) + 0x80,
                          clut, 1, glow, 0);
            break;
        }
        case 2:
            glow = func_80077DC4((s16)p->timer << 7) / 32;
            scale = func_80077CF4((s16)p->timer << 7) / 4 + 0xC00;
            radius = p->radius * scale / 4096;
            size = func_80077DC4((s16)p->timer << 7) * 240 / 4096;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800D0E88(&D_801994EC, &p->heading, radius, size, &seed, 0, 0,
                          (s16)glow, 1);
            break;
        }
        break;
    }
    return 0;
}
