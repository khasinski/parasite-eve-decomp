#include "pe1/gte.h"
#include "pe1/room_m123.h"
#include "pe1/room_m123_glow_burst.h"
#include "room_m123_effects.h"

int func_80192BDC(int mode, RoomM123PulsingParticle *particle) {
    GteShortVector position;
    GteShortVector output;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    int blend;
    u16 clut;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 16;
        if ((s16)particle->frame >= 48) return 1;
        break;
    case 2:
        scale = particle->scale *
                func_80077DC4(((s16)particle->frame << 10) / 48) / 4096;
        func_800CF844(&D_80195690, &position, scale / 2, &D_80195688,
                      scale, (s16)particle->offset);
        if ((s16)particle->frame < 16) {
            func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20(&position, 0, 0x1000, 0x1000,
                          D_800F336A * 2 + 0xD8, clut, 3, D_80195684,
                          &color);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D1DEC(&position, &color, D_80195684, 1);
            /* Retail re-tests the frame here; the blended tail only runs
             * from frame 16 on. */
            if ((s16)particle->frame < 16) break;
        }
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
        blend = func_80077DC4(((s16)particle->frame - 16) << 5);
        func_800783E4(&position, &D_80195690, blend, 0x1000 - blend, &output);
        func_800D2B58(&output, &position, &color, 0, D_80195684, 0, 1);
        break;
    }
    return 0;
}

/* Glow burst: spawns sixteen pulsing particles at frame 1, then draws four
 * glow layers, two flash rings and a swept flare on joint 23. The parameter
 * block fields written twice share a base register; the tile indices are
 * read as D_800E11E4 elements so their loads stay below the base-register
 * stores, and the palette read-back follows the block's stores. */

int func_80192F0C(int mode, RoomM123GlowBurst *burst) {
    RoomM123BurstTemplate template = D_8018F1E0;
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    RoomM123BurstParticle *child;
    int i;
    int intensity;
    int step;
    int fade;

    switch (mode) {
    case 0:
        burst->timer = 0;
        return func_800CE560(D_800F33E0->pool, 8, 0x10, func_80192BDC);
    case 1:
        if (burst->timer < 0x80) burst->timer += 4;
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->frame = 0;
                    child->scale = (func_80071A54() & 0x1FF) + 0x200;
                    child->offset = func_80071A54();
                }
            }
            func_800D3F64(0x580, func_800D3FD8());
            func_800D3F64(0x5BD, 0x80);
        }
        if (D_800E27EC >= 33) return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x32;
        func_800CE8F0(D_800F32D0->pool, 0x17, &template, &position);
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -D_800E27EC << 6;
        rotation.flags = 0;
        intensity = burst->timer;
        if (D_800E27EC != 0) intensity = intensity * 2 / 3;
        func_80077DC4((D_800E27EC << 10) / 40);
        {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800D3BC8(&position, 0x1000, 0x1000, 0x50,
                          func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                          1, intensity, 0, 0);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x4D, func_80077AA4(0x50, palette),
                          1, intensity, 0, 0xA00);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x48, func_80077AA4(0x30, palette),
                          1, intensity, 0, 0xC00);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x49, func_80077AA4(0x40, palette),
                          1, intensity, 0, 0xE00);
        }
        func_800CF3AC(D_801954BC, &color, D_800E27EC / 2);
        func_800D004C(&position, 700, 700, 0x10, 0, 0x1000, 0x1000, &color, 0, 0x40, 1);
        func_800CF3AC(D_801954E4, &color, D_800E27EC * 48 / 40);
        func_800D004C(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        D_80195684 = burst->timer;
        D_80195690.x = position.x;
        D_80195690.y = position.y;
        D_80195690.z = position.z;
        func_800CE9D4(D_800F32D0->pool, 0, &D_80195688.vector);
        step = D_800E27EC - 16;
        if ((unsigned int)step < 17) {
            fade = func_80077DC4(step << 6);
            func_800CF3AC(D_801954E4, &color, step * 3);
            D_80195688.rotation.flags = 1;
            func_800D0728(&position, 700, 1000, 0x18, &D_80195688.rotation, fade, fade, 0, &color,
                          0x80, 1);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.tpage = D_800E2850[D_800E11E8];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 0xC;
        break;
    }
    return 0;
}
