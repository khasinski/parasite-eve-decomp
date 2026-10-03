#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_WAVE_SPARK_FUNC
#error "ROOMEFFECT_WAVE_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_WAVE_SPARK_ROTATION_A
#error "ROOMEFFECT_WAVE_SPARK_ROTATION_A must name the first rotation seed"
#endif
#ifndef ROOMEFFECT_WAVE_SPARK_ROTATION_B
#error "ROOMEFFECT_WAVE_SPARK_ROTATION_B must name the second rotation seed"
#endif
#ifndef ROOMEFFECT_WAVE_SPARK_ROTATION_C
#error "ROOMEFFECT_WAVE_SPARK_ROTATION_C must name the third rotation seed"
#endif
#ifndef ROOMEFFECT_WAVE_SPARK_COLOR_TABLE
#error "ROOMEFFECT_WAVE_SPARK_COLOR_TABLE must name the colour ramp table"
#endif

extern GteRotation ROOMEFFECT_WAVE_SPARK_ROTATION_A;
extern GteRotation ROOMEFFECT_WAVE_SPARK_ROTATION_B;
extern GteRotation ROOMEFFECT_WAVE_SPARK_ROTATION_C;
extern u8 ROOMEFFECT_WAVE_SPARK_COLOR_TABLE[];

/* Small flash drawn at the projected position during the first frames. */
#define ROOMEFFECT_WAVE_SPARK_FLASH()                                        \
    if ((s16)spark->timer < 4) {                                             \
        u16 clut;                                                            \
        int kind;                                                            \
        int palette;                                                         \
        D_800F3368.parameter00 = 0x10;                                       \
        D_800F3368.parameter02 = 1;                                          \
        D_800F3368.extent_x = 0x10;                                          \
        D_800F3368.extent_y = 0x10;                                          \
        kind = D_800F3368.palette;                                           \
        palette = D_800E1204[kind];                                          \
        if (kind == 4 && D_800F3428 != 0) palette += 4;                      \
        clut = func_80077AA4(0x40, palette);                                 \
        func_800CEE20(&position, 0, 0x2000, 0x2000,                          \
                      (s16)D_800F3368.parameter02 * (spark->timer & 3) + 0xA3, \
                      clut, 1, 0x80, 0);                                     \
    }

/* Larger sine-scaled sprite at the projected position. */
#define ROOMEFFECT_WAVE_SPARK_BLOOM(recompute)                               \
    {                                                                        \
        u16 clut;                                                            \
        int kind;                                                            \
        int palette;                                                         \
        D_800F3368.parameter00 = 0x20;                                       \
        D_800F3368.parameter02 = 2;                                          \
        D_800F3368.extent_x = 0x20;                                          \
        D_800F3368.extent_y = 0x20;                                          \
        if (recompute) {                                                     \
            fade = func_80077DC4(((s16)spark->timer << 10) / 24) / 32;       \
            scale = func_80077CF4(((s16)spark->timer << 10) / 24) + 0x800;   \
        }                                                                    \
        kind = D_800F3368.palette;                                           \
        palette = D_800E1204[kind];                                          \
        if (kind == 4 && D_800F3428 != 0) palette += 4;                      \
        clut = func_80077AA4(0x60, palette);                                 \
        func_800CEE20(&position, 0, scale, scale, 0xE, clut, 1, fade, 0);    \
    }

/* Spark that rides the effect channel's wave slots: it projects itself
 * along the current and next slot, flashes briefly, then collapses into a
 * floor sprite and a coloured burst between the two projected points. */
int ROOMEFFECT_WAVE_SPARK_FUNC(int mode, RoomPhasedSpark *spark) {
    GteShortVector position;
    GteShortVector second;
    GteRotation rotationA = ROOMEFFECT_WAVE_SPARK_ROTATION_A;
    GteRotation rotationB = ROOMEFFECT_WAVE_SPARK_ROTATION_B;
    GteRotation rotationC = ROOMEFFECT_WAVE_SPARK_ROTATION_C;
    RenderColor color;
    GteShortVector floorPos;
    GteRotation floorSpin;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int fade;
    int phase;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            if ((s16)spark->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0:
            phase = spark->phase;
            if (phase < 7) {
                func_800CE8F0(D_800F32D0->pool, phase + 1, &rotationA, &position);
                ROOMEFFECT_WAVE_SPARK_FLASH();
                ROOMEFFECT_WAVE_SPARK_BLOOM(1);
                func_800CE8F0(D_800F32D0->pool, spark->phase + 1, &rotationB, &position);
            } else {
                func_800CE8F0(D_800F32D0->pool, phase + 9, &rotationC, &position);
                ROOMEFFECT_WAVE_SPARK_FLASH();
                ROOMEFFECT_WAVE_SPARK_BLOOM(1);
                func_800CE8F0(D_800F32D0->pool, spark->phase + 13, &rotationC, &position);
            }
            ROOMEFFECT_WAVE_SPARK_FLASH();
            ROOMEFFECT_WAVE_SPARK_BLOOM(0);
            break;
        case 1:
            func_800CE8F0(D_800F32D0->pool, 11, &rotationC, &position);
            func_800CE8F0(D_800F32D0->pool, 15, &rotationC, &second);
            {
                u16 clut;
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x10;
                D_800F3368.parameter02 = 1;
                D_800F3368.extent_x = 0x10;
                D_800F3368.extent_y = 0x10;
                position.x = (position.x + second.x) / 2;
                position.y = (position.y + second.y) / 2;
                position.z = (position.z + second.z) / 2;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800CEE20(&position, 0, 0x2000, 0x2000,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0xA7,
                              clut, 1, 0x80, 0);
            }
            {
                u16 clut;
                int kind;
                int palette;
                floorSpin.x = 0x400;
                floorSpin.y = 0;
                floorSpin.z = 0;
                floorSpin.flags = 1;
                floorPos.x = position.x;
                floorPos.z = position.z;
                floorPos.y = D_800942EC.count;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800CEE20(&floorPos, &floorSpin, 0x1000, 0x1000,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0xA7,
                              clut, 3, 0x80, 0);
            }
            scale = func_80077DC4((s16)spark->timer << 7);
            func_800CF3AC(ROOMEFFECT_WAVE_SPARK_COLOR_TABLE, &color, (s16)spark->timer);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D0728(&position, 0x15E, 0x2BC, 0x18, 0, scale, scale, 0, &color,
                          0x80, 1);
            break;
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_WAVE_SPARK_FLASH
#undef ROOMEFFECT_WAVE_SPARK_BLOOM
#undef ROOMEFFECT_WAVE_SPARK_FUNC
#undef ROOMEFFECT_WAVE_SPARK_ROTATION_A
#undef ROOMEFFECT_WAVE_SPARK_ROTATION_B
#undef ROOMEFFECT_WAVE_SPARK_ROTATION_C
#undef ROOMEFFECT_WAVE_SPARK_COLOR_TABLE
