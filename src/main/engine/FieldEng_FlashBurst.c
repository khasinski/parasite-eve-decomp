#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_flash_burst.h"

/* Mode 0 anchors at actor point 19; mode 1 runs 25 frames; mode 2 flashes
 * the screen, swells the model for 16 frames and draws the fan, band and
 * quad while the burst lasts. */
int func_800DE0A8(int mode, FieldFlashBurst *state)
{
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;

    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, state->point);
        state->anchor.x = D_8009D254->render_object.matrices[19].translation[0];
        state->anchor.y = D_8009D254->render_object.matrices[19].translation[1];
        state->anchor.z = D_8009D254->render_object.matrices[19].translation[2];
        func_800C6D5C(D_800F3474, 0, 0);
        break;
    case 1:
        if (D_800E27EC >= 25)
            return 1;
        break;
    case 2:
        if (D_800E27EC == 0) {
            *(u32 *)&color = 0xA0A0A0;
            func_800D1AE0(&color.r, 0x50, 2, 8);
        }
        if (D_800E27EC >= 1 && D_800E27EC < 6) {
            *(u32 *)&color = 0xA0FFFF;
            func_800D1AE0(&color.r, 0x80 - (D_800E27EC - 1) * 32, 1, 8);
        }
        if (D_800E27EC < 17) {
            matrix = *(GteMatrix *)&D_8009D254->render_object.matrices[0];
            scale.x = scale.y = rcos(D_800E27EC << 6) / 3;
            scale.z = rsin(D_800E27EC << 6);
            Gte_ScaleMatrix(&matrix, &scale);
            matrix.t[0] = D_8009D254->render_object.matrices[19].translation[0];
            matrix.t[1] = D_8009D254->render_object.matrices[19].translation[1];
            matrix.t[2] = D_8009D254->render_object.matrices[19].translation[2];
            func_800CF3AC(D_800E20CC, &color, D_800E27EC);
            slots = &D_800E11E6;
            D_800F3368.tpage = D_800E2850[slots[0]];
            D_800F3368.palette = 1;
            func_800CEDA8(1);
            tpages = D_800E2850;
            D_800F3368.parameter06 = 0;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                int tpage = (u16)(tpages[slots[0]] | GetTPage(0, 1, 0, 0));
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428)
                    palette += 4;
                GsSetOrign(tpage, GetClut(0xD0, palette));
            }
            func_800C6ED8(1);
            func_800C6EF8(D_800F3474);
            func_800C7098(D_800F3474, color.r, color.g, color.b);
            func_800C71E4(D_800F3474, &matrix);
            scale.x = scale.y = 0x1800;
            scale.z = 0x1000;
            Gte_ScaleMatrix(&matrix, &scale);
            func_800C7098(D_800F3474, color.r, color.g >> 1, 0);
            {
                int tpage = (u16)(D_800E2850[D_800E11E6] | GetTPage(0, 1, 0, 0));
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428)
                    palette += 4;
                GsSetOrign(tpage, GetClut(0xD0, palette));
            }
            func_800C71E4(D_800F3474, &matrix);
            func_800C6F4C(D_800F3474);
        }
        if (D_800E27EC < 25) {
            D_800F3368.depth = 16;
            func_800CF3AC(D_800E20CC, &color, (D_800E27EC << 4) / 24);
            func_800D004C((GteShortVector *)state, 600, 600, 12, 0, 0x1000, 0x1000,
                          &color, 0, 0x80, 1);
            if (D_800E27EC < 17) {
                color.b = 0;
                size = rsin(D_800E27EC << 6) + 0x800;
                func_800CF3AC(D_800E20CC, &color, D_800E27EC);
                func_800D0728((GteShortVector *)state, 500, 600, 20, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            intensity = rcos((D_800E27EC << 10) / 24) / 32;
            *(u32 *)&color = 0xC8B4B4;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11F6];
            D_800F3368.palette = 1;
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            {
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                func_800CEE20((GteShortVector *)state, 0, 0x2000, 0x2000, 0x42,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                              1, intensity, &color);
            }
        }
        break;
    }
    return 0;
}
