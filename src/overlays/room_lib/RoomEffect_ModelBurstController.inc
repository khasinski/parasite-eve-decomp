#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_MODEL_BURST_FUNC
#error "ROOMEFFECT_MODEL_BURST_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_CALLBACK
#error "ROOMEFFECT_MODEL_BURST_CALLBACK must name the particle callback"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_ROTATION
#error "ROOMEFFECT_MODEL_BURST_ROTATION must name the rotation seed"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_SHADE
#error "ROOMEFFECT_MODEL_BURST_SHADE must name the four-byte shade seed"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_COLOR_TABLE
#error "ROOMEFFECT_MODEL_BURST_COLOR_TABLE must name the colour ramp table"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_ASSET
#error "ROOMEFFECT_MODEL_BURST_ASSET must name the model asset pointer"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_LAYER
#error "ROOMEFFECT_MODEL_BURST_LAYER must name the layer record"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_ANCHOR
#error "ROOMEFFECT_MODEL_BURST_ANCHOR must name the anchor position record"
#endif
#ifndef ROOMEFFECT_MODEL_BURST_SLOT
#error "ROOMEFFECT_MODEL_BURST_SLOT must give the wave slot the model follows"
#endif

typedef struct RoomModelBurstState {
    s16 brightness;               /* 0x00 */
} RoomModelBurstState;

typedef struct RoomModelBurstParticle {
    s16 x, y, z, reserved06;      /* 0x00 */
    s16 field08;
    s16 field0A;
    s16 field0C;
    s16 field0E;
} RoomModelBurstParticle;

typedef struct RoomModelBurstMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} RoomModelBurstMatrix;

typedef struct RoomModelBurstScale {
    s32 x, y, z, reserved0C;
} RoomModelBurstScale;

typedef struct RoomModelBurstAnchor {
    s16 x, y, z;
} RoomModelBurstAnchor;

extern GteRotation ROOMEFFECT_MODEL_BURST_ROTATION;
extern RenderColor ROOMEFFECT_MODEL_BURST_SHADE;
extern u8 ROOMEFFECT_MODEL_BURST_COLOR_TABLE[];
extern void *ROOMEFFECT_MODEL_BURST_ASSET;
extern u8 ROOMEFFECT_MODEL_BURST_LAYER[];
extern RoomModelBurstAnchor ROOMEFFECT_MODEL_BURST_ANCHOR;
extern int ROOMEFFECT_MODEL_BURST_CALLBACK(int mode, RoomModelBurstParticle *particle);

extern void *D_800B0E64;
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern void func_800CF4B4(int kind, int value, void *layer);
extern s32 func_80077A64(s32, s32, s32, s32);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int);
extern void func_80079754(void *rotation, void *matrix);
extern void func_80078CC4(void *matrix, void *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C7098(void *asset, int r, int g, int b);
extern void func_800C6FA0(void *asset, int brightness);
extern void func_800C71E4(void *asset, void *matrix);
extern void func_800C6F4C(void *asset);
extern RoomModelBurstParticle *func_800CE610_burst(void *pool) __asm__("func_800CE610");

/* Controller that loads a model asset, spawns one particle per frame while
 * brightening, and draws a growing sprite pair, the actor's sprite and the
 * scaled model at the projected anchor. */
int ROOMEFFECT_MODEL_BURST_FUNC(int mode, s16 *state) {
    GteRotation rotation = ROOMEFFECT_MODEL_BURST_ROTATION;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RenderColor shade = ROOMEFFECT_MODEL_BURST_SHADE;
    RoomModelBurstMatrix matrix;
    RoomModelBurstScale scale;
    RoomModelBurstParticle *child;
    u16 *tpages;
    int brightness;
    int size;
    int handle;

    switch (mode) {
    case 0:
        *state = 0;
        ROOMEFFECT_MODEL_BURST_ASSET = func_8006E498(D_800B0E64, 0xC54A5704);
        func_800C6D5C(ROOMEFFECT_MODEL_BURST_ASSET, 0, 0);
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        func_800CF4B4(3, -1, ROOMEFFECT_MODEL_BURST_LAYER);
        handle = func_800D3FD8();
        func_800D3F64(0x5A4, handle);
        return func_800CE560(D_800F33E0->pool, 0x10, 0x10, ROOMEFFECT_MODEL_BURST_CALLBACK);
    case 1:
        if (*state < 0x80) *state += 8;
        if (D_800E27EC >= 2) {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            func_800CF4B4(3, D_800E27EC * 2, ROOMEFFECT_MODEL_BURST_LAYER);
        }
        child = func_800CE610_burst(D_800F33E0->pool);
        if (child) {
            child->x = func_80071A54();
            child->y = func_80071A54();
            child->z = func_80071A54();
            child->field0A = 0;
            child->field08 = 0;
            child->field0C = (func_80071A54() & 0x7F) - 0x40;
            child->field0E = (func_80071A54() & 0x7F) - 0x40;
        }
        if (D_800E27EC < 0x30) break;
        return 1;
    case 2:
        {
            int tpage;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            tpage = D_800E2850[D_800E11FA];
            D_800F3368.parameter06 = 1;
            D_800F3368.palette = 3;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        func_800CE8F0(D_800F32D0->pool, ROOMEFFECT_MODEL_BURST_SLOT, &rotation, &position);
        spin.x = func_80077CF4(D_800E27EC * 80) / 4;
        spin.y = func_80077DC4(D_800E27EC << 7) / 8;
        spin.flags = 0;
        spin.z = -D_800E27EC << 3;
        brightness = *state;
        if (D_800E27EC != 0) brightness = brightness * 2 / 3;
        size = func_80077CF4((D_800E27EC << 10) / 48) / 4 + 0x400;
        func_800CF3AC(ROOMEFFECT_MODEL_BURST_COLOR_TABLE, &color, D_800E27EC * 3);
        func_800D004C(&position, 0x124, 0x124, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness, 1);
        func_800D0728(&position, 0xC8, 0x1F4, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness / 2, 1);
        ROOMEFFECT_MODEL_BURST_ANCHOR.x = position.x;
        ROOMEFFECT_MODEL_BURST_ANCHOR.y = position.y;
        ROOMEFFECT_MODEL_BURST_ANCHOR.z = position.z;
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        func_800CF3AC(ROOMEFFECT_MODEL_BURST_COLOR_TABLE, &color, D_800E27EC);
        {
            u16 clut;
            int kind;
            int palette;
            int page;
            int tpage;
            tpages = D_800E2850;
            tpage = tpages[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
            page = (tpages[D_800E11EA] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800C6EC0(page, clut);
        }
        func_800C6ED8(1);
        func_80079754(&spin, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = size;
        scale.y = size;
        scale.z = size;
        func_80078CC4(&matrix, &scale);
        func_800C6EF8(ROOMEFFECT_MODEL_BURST_ASSET);
        func_800C7098(ROOMEFFECT_MODEL_BURST_ASSET, color.r, color.g, color.b);
        func_800C6FA0(ROOMEFFECT_MODEL_BURST_ASSET, (u16)(brightness / 2));
        func_800C71E4(ROOMEFFECT_MODEL_BURST_ASSET, &matrix);
        func_800C6F4C(ROOMEFFECT_MODEL_BURST_ASSET);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_MODEL_BURST_FUNC
#undef ROOMEFFECT_MODEL_BURST_CALLBACK
#undef ROOMEFFECT_MODEL_BURST_ROTATION
#undef ROOMEFFECT_MODEL_BURST_SHADE
#undef ROOMEFFECT_MODEL_BURST_COLOR_TABLE
#undef ROOMEFFECT_MODEL_BURST_ASSET
#undef ROOMEFFECT_MODEL_BURST_LAYER
#undef ROOMEFFECT_MODEL_BURST_ANCHOR
#undef ROOMEFFECT_MODEL_BURST_SLOT
