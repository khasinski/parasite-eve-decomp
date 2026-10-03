#include "pe1/scene_e22_ring_burst.h"
#include "pe1/gte.h"

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
