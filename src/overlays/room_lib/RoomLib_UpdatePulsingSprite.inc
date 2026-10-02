#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"
#include "pe1/render_object.h"

/* Drifting sprite that pulses with a cosine of its frame counter. Mode 1
 * advances it for sixteen frames; mode 2 draws a textured rectangle and a
 * scaled sprite at a fixed room position. */
typedef struct RoomPulsingSpriteState {
    unsigned short x;             /* 0x00 */
    unsigned short y;             /* 0x02 */
    unsigned short z;             /* 0x04 */
    short pad06;                  /* 0x06 */
    short pad08;                  /* 0x08 */
    unsigned short frame;         /* 0x0A */
    unsigned short vx;            /* 0x0C */
    unsigned short vy;            /* 0x0E */
} RoomPulsingSpriteState;

extern RenderColor ROOMLIB_UPDATE_PULSING_SPRITE_COLOR;
extern GteShortVector ROOMLIB_UPDATE_PULSING_SPRITE_POSITION;

extern int func_80077DC4(int angle);
extern int func_80077AA4(int, int);

int ROOMLIB_UPDATE_PULSING_SPRITE_NAME(int mode, RoomPulsingSpriteState *state) {
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    u16 clut;

    color = ROOMLIB_UPDATE_PULSING_SPRITE_COLOR;
    switch (mode) {
    case 1:
        state->frame++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += 0x18;
        if ((short)state->frame >= 16) return 1;
        break;
    case 2:
        scale = func_80077DC4((short)state->frame << 6) / 32;
        func_80077DC4((short)state->frame << 6);
        u = (D_800E27EC & 1) << 7;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x10, palette);
        func_800D2370(&ROOMLIB_UPDATE_PULSING_SPRITE_POSITION,
                      (GteRotation *)state, 0x1FE,
                      ROOMLIB_UPDATE_PULSING_SPRITE_HEIGHT, u, 0, 0x80,
                      0x20, clut, &color, &color, (short)scale, 1);
        kind = D_800F336C;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = func_80077AA4(0x40, palette);
        func_800CEE20(&ROOMLIB_UPDATE_PULSING_SPRITE_POSITION, 0, 0x1000,
                      0x1000, D_800F336A * ((short)state->frame / 2) + 0x90,
                      clut, 1, scale / 2, 0);
        break;
    }
    return 0;
}
