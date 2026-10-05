#include "pe1/scene_e19_2_twin_glow.h"

/* Mode 0 picks the two joints; mode 1 sprays a ring of sixteen sparks and
 * sixteen falling sparks at frame 15 and reports 2 after frame 33; mode 2
 * draws pulsing glows on both joints, then fading glows, a ring and a
 * floor glow. */
int func_8019A054(int mode, SceneTwinGlow *glow)
{
    GteShortVector jointA;
    GteShortVector jointB;
    GteRotation ring = D_8018F1D4;
    GteShortVector offset = D_8018F264;
    RenderColor color = D_8018F26C;
    GteShortVector floor;
    GteRotation flat;
    RoomOrbitTrailParticle *child;
    int i;
    int scale;
    int fade;
    int time;

    switch (mode) {
    case 0:
        glow->jointA = 6;
        glow->jointB = 0x12;
        glow->state = 0;
        glow->timer = 0;
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_80199C28);
    case 1:
        glow->timer++;
        if (glow->timer == 0xF) {
            for (i = 0; i < 16; i++) {
                time = i << 8;
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = glow->position.x;
                    child->y = glow->position.y;
                    child->z = glow->position.z;
                    child->x += (func_80071A54() & 0xFF) - 0x80;
                    child->z += (func_80071A54() & 0xFF) - 0x80;
                    child->heading.x = func_80077DC4(time) * 60 / 4096;
                    child->heading.z = func_80077CF4(time) * 60 / 4096;
                    child->heading.y = -(func_80071A54() & 0xF) - 8;
                    child->state = 0;
                    child->timer = 0;
                }
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = glow->position.x;
                    child->y = glow->position.y;
                    child->z = glow->position.z;
                    child->heading.x = func_80071A54() % 49 - 0x18;
                    child->heading.y = func_80071A54() % 128 - 0x40;
                    child->heading.z = func_80071A54() % 49 - 0x18;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (glow->timer < 0x22) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CE8F0(D_800F32D0->pool, glow->jointA, &offset, &jointA);
        func_800CE8F0(D_800F32D0->pool, glow->jointB, &offset, &jointB);
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA.value];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (glow->timer < 0xE) {
            scale = func_80077CF4(glow->timer << 5) / 2 + 0x800;
            if (glow->timer < 4) fade = glow->timer << 5;
            else fade = ((glow->timer & 1) << 6) + 0x80;
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              func_80077AA4(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale, scale, 8, func_80077AA4(0x10, palette), 3,
                              fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              func_80077AA4(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale, scale, 8, func_80077AA4(0x10, palette), 3,
                              fade, 0);
            }
        } else if (glow->timer < 0x23) {
            if (glow->timer == 0xE) {
                glow->position.x = jointA.x;
                glow->position.y = jointA.y;
                glow->position.z = jointA.z;
                glow->position.y = D_800942EC.count;
            }
            time = glow->timer - 0xE;
            scale = func_80077CF4((time << 10) / 20) + 0x1000;
            fade = 0x80 - (time << 7) / 20;
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              func_80077AA4(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale, scale, 8, func_80077AA4(0x10, palette), 3,
                              fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              func_80077AA4(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale, scale, 8, func_80077AA4(0x10, palette), 3,
                              fade, 0);
            }
            func_800D0728(&glow->position, 800, 400, 0x14, &ring, scale, scale, &color, 0,
                          fade / 2, 1);
            scale = func_80077CF4(time * 1248 / 20 + 800) + 0x800;
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.flags = 1;
            floor.x = glow->position.x;
            floor.z = glow->position.z;
            floor.y = D_800942EC.count;
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&floor, &flat, scale, scale, 8, func_80077AA4(0x10, palette), 1,
                              fade * 2 / 3, 0);
            }
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}
