#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_drift_pulse.h"

/* Drift/pulse sprite for room_m023. The glow strip's anchor is taken into
 * a local pointer first: its address load is what keeps the frame counter
 * load after the palette lookup like retail. */

int func_8018F004(int mode, RoomDriftPulseState *state) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC >= 4) {
                return 1;
            }
        case 1:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC < 8) {
                break;
            }
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0:
            {
                GteShortVector *anchor = &D_80190758;
                int kind;
                int palette;
                int fade;
                u16 clut;

                *(int *)&color = 0x808080;
                origin.x = state->x;
                origin.y = state->y;
                origin.z = state->z;
                kind = D_800F336C;
                palette = D_800E1204[kind];
                fade = (D_800E27EC - 1) << 4;
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0, palette);
                func_800D2370(anchor, (GteRotation *)state, 0x3C0, 0x64, 0, fade,
                              0xFF, 0xF, clut, 0, &color, 0xBE, 1);
            }
            rotation.x = state->x;
            rotation.y = state->y;
            rotation.z = state->z;
            rotation.flags = 1;
            {
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x20, palette);
                func_800CEE20(&D_80190758, &rotation, 0x1000, 0x1000,
                              (D_800F336A << 1) * (D_800E27EC - 1) + 0x60, clut,
                              3, 0x80, 0);
            }
            break;
        case 1:
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = D_800E27EC << 6;
            {
                int scale = func_80077DC4((D_800E27EC - 1) << 7) / 32;
                int kind = D_800F336C;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x10, palette);
                func_800CEE20((GteShortVector *)state, &rotation, 0x1000, 0x1000,
                              D_800F336A * D_800E27EC + 0x40, clut, 1, scale, 0);
            }
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}
