#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_model_pulse.h" /* copy from this directory */

/* Controller: loads the model asset, seeds sixteen ring particles on frame
 * 1, then draws the slot-2 ring sprite and, from frame 4, the model scaled by
 * cos and brightened by sin of the elapsed time. */
int func_80195728(int mode, s16 *state) {
    GteRotation rotation = D_8018F258;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RoomModelPulseMatrix matrix;
    RoomModelPulseScale scale;
    RoomModelPulseParticle *child;
    u16 *tpages;
    int palette3;
    int i;

    switch (mode) {
    case 0:
        *state = 0;
        D_801960A8 = func_8006E498(D_800B0E64, 0xC5487704);
        func_800C6D5C(D_801960A8, 0, 0);
        return func_800CE560(D_800F33E0->pool, 8, 0x10, func_8019552C);
    case 1:
        if (*state < 0x80) *state += 4;
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                child = func_800CE610_pulse(D_800F33E0->pool);
                if (child) {
                    child->frame = 0;
                    child->scale = (func_80071A54() & 0x1FF) + 0x200;
                    child->offset = func_80071A54();
                }
            }
        }
        if (D_800E27EC < 0x10) break;
        return 1;
    case 2:
        palette3 = 3;
        {
            int tpage;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            tpage = D_800E2850[D_800E11FA];
            D_800F3368.parameter06 = 1;
            D_800F3368.palette = palette3;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        func_800CE8F0(D_800F32D0->pool, 2, &rotation, &position);
        tpages = D_800E2850;
        spin.x = 0;
        spin.y = 0;
        spin.z = -D_800E27EC << 6;
        spin.flags = 0;
        func_80077DC4((D_800E27EC << 10) / 40);
        func_800CF3AC(D_80195E8C, &color, D_800E27EC * 3);
        func_800D004C(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0,
                      0x80, 1);
        D_801960A0.x = position.x;
        D_801960A0.y = position.y;
        D_801960A0.z = position.z;
        D_80196094 = *state;
        func_800CE9D4(D_800F32D0->pool, 2, &D_80196098);
        D_80196098.x += 0x400;
        if (D_800E27EC >= 4) {
            int angle;
            int size;
            int brightness;

            angle = ((D_800E27EC - 4) << 10) / 12;
            size = func_80077DC4(angle);
            brightness = func_80077CF4(angle) / 32;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            {
                u16 clut;
                int kind;
                int palette;
                int page;
                int tpage;
                tpage = tpages[D_800E11EA];
                D_800F3368.palette = palette3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
                page = (tpages[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800C6EC0(page, clut);
            }
            func_800C6ED8(1);
            func_80079754(&D_80196098, &matrix);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            func_80078CC4(&matrix, &scale);
            func_800C6EF8(D_801960A8);
            func_800C6FA0(D_801960A8, (u16)brightness);
            func_800C71E4(D_801960A8, &matrix);
            func_800C6F4C(D_801960A8);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_80196098.x += 0x600;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 0xC;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}
