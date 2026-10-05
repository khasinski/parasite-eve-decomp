#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_swirl_rise.h"

/* Swirl/rise sprite for room_m089. Both draws pass their intensity through
 * one function-scope variable (two sets keep each computation where retail
 * has it), and the second draw takes the state as its rotation through a
 * local pointer, whose copy keeps the frame counter load below the palette
 * lookup. */

int RoomEffect_SwirlRiseSprite_80193200(int mode, RoomSwirlRiseState *state,
                                        GteShortVector *anchor) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int intensity;

    color = D_8018F1D8;
    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0: {
            int radius;

            state->x = anchor->x;
            state->y = anchor->y;
            state->z = anchor->z;
            radius = state->radius * D_800E27EC / 64;
            state->x += func_80077DC4(state->angle) * radius / 4096;
            state->z += func_80077CF4(state->angle) * radius / 4096;
            state->y -= func_80077CF4(D_800E27EC << 5) * 120 / 4096;
            state->angle += 12;
            if (D_800E27EC >= 64) {
                return 1;
            }
            break;
        }
        case 1:
            state->x += 0x20;
            if (D_800E27EC >= 6) {
                return 1;
            }
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0: {
            int scale;
            int kind;
            int palette;
            int clut;

            rotation.x = 0;
            rotation.y = 0;
            rotation.z = state->angle + D_800E27EC * 12 + state->radius;
            rotation.flags = 0;
            intensity = 0x80 - D_800E27EC * 2;
            scale = func_80077CF4(D_800E27EC << 4) + 0x800;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = (u16)func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)state, &rotation, scale, scale,
                          D_800F336A * (D_800E27EC / 16) + 0x24, clut, 1,
                          intensity, 0);
            return 0;
        }
        case 1: {
            int v;
            GteRotation *spin;
            int kind;
            int palette;
            u16 clut;

            origin.x = anchor->x;
            origin.y = anchor->y;
            origin.z = anchor->z;
            intensity = func_80077DC4((D_800E27EC << 10) / 6) / 32;
            *(int *)&color = 0x808080;
            spin = (GteRotation *)state;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            v = ((D_800E27EC & 1) << 4) + 0x80;
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800D2370(&origin, spin, 0x3C0, 0x8C, 0, v, 0xFF,
                          0xF, clut, &color, 0, (s16)intensity, 1);
            break;
        }
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}
