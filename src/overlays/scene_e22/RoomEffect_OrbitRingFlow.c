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

/* Ring burst: while the scene event runs it bursts falling sparks, an
 * optional volley of sweeping trails and drifting rings from the anchor
 * during the first three frames, and flashes a glow, a halo and a tilted
 * ring while it fades; the anchor is published as the orbit centre. */
int func_80192AF0(int mode, RoomOrbitRingBurst *burst) {
    GteRotation tilt;
    RenderColor ringColor = D_8018F1D0;
    RenderColor haloColor = D_8018F1D4;
    RoomOrbitTrailParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->counterB = 0;
        burst->counterA = 0;
        burst->sweep = 1;
        func_800CE870(*(char **)RoomMain_ActorPtr, 0, (s16 *)burst);
        return func_800CE560(D_800F33E0->pool, 0x14, 0x18, func_80192548);
    case 1:
        if (D_800E2368->running == 0)
            return 1;
        if (D_800E27EC == 2) {
            func_800D3F64(0x5D4, func_800D3FD8());
            func_800D3F64(0x5D5, 0x80);
        }
        if (D_800E27EC < 3) {
            for (i = 0; i < 2; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54() % 60 - 0x1E;
                    child->heading.y = func_80071A54() % 60 - 0x1E;
                    child->heading.z = func_80071A54() % 60 - 0x1E;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (burst->sweep) {
                for (i = 0; i < 5; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = burst->x;
                        child->y = burst->y;
                        child->z = burst->z;
                        child->heading.x = func_80071A54();
                        child->heading.y = func_80071A54();
                        child->heading.z = func_80071A54();
                        child->radius = (func_80071A54() & 0x3FF) + 0x200;
                        child->state = 0;
                        child->timer = 0;
                    }
                }
            }
            for (i = 0; i < 5; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = func_80071A54();
                    child->heading.y = func_80071A54();
                    child->heading.z = func_80071A54();
                    child->radius = (func_80071A54() & 0x7FF) + 0x400;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 0x1C) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (D_800E27EC < 0x15) {
            int kind;
            int palette;
            int glow = func_80077DC4((D_800E27EC << 10) / 20) / 32;
            int size = func_80077CF4((D_800E27EC << 10) / 20) + 0x1000;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            palette = func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 7 : palette + 3);
            func_800CEE20((GteShortVector *)burst, 0, size, size, 4, (u16)palette, 1,
                          glow / 2, 0);
            func_800D004C((GteShortVector *)burst, 800, 800, 12, 0, 0x1000, 0x1000,
                          &haloColor, 0, glow, 1);
            tilt.x = 0x400;
            tilt.y = 0;
            tilt.z = 0;
            tilt.flags = 1;
            /* The ring scale reuses the glow sprite's size temporary. */
            size = func_80077CF4((D_800E27EC << 10) / 20);
            func_800D0728((GteShortVector *)burst, 0x44C, 0x578, 0x14, &tilt, size,
                          size, 0, &ringColor, glow / 2, 1);
        }
        D_801994EC.x = burst->x;
        D_801994EC.y = burst->y;
        D_801994EC.z = burst->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
