#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m005.h"

/* Sprite that drifts by a fixed velocity for six frames, then draws itself
 * at a room offset with a cosine-driven intensity. */
int func_8018FB84(int mode, RoomM005DriftingSpriteState *state) {
    GteShortVector position;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    int v;
    u16 clut;

    color = D_8018F00C;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        if (D_800E27EC >= 6) return 1;
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        position.x += D_80190BA0;
        position.y += D_80190BA2;
        position.z += D_80190BA4;
        u = (D_800E27EC & 1) << 7;
        v = ((D_800E27EC << 3) & 0x10) + 0x20;
        scale = func_80077DC4((D_800E27EC << 10) / 6) / 24;
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0, palette);
        func_800D2370(&D_80190BA8, (GteRotation *)&position, 0x44C, 0x6E, u, v,
                      0x7F, 0xF, clut, &color, &color, (short)scale, 1);
        break;
    }
    return 0;
}
