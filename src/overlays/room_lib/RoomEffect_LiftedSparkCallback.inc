#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_LIFTED_SPARK_FUNC
#error "ROOMEFFECT_LIFTED_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_LIFTED_SPARK_ROTATION
#error "ROOMEFFECT_LIFTED_SPARK_ROTATION must name the rotation seed"
#endif
#ifndef ROOMEFFECT_LIFTED_SPARK_COLOR_TABLE
#error "ROOMEFFECT_LIFTED_SPARK_COLOR_TABLE must name the colour ramp table"
#endif

extern GteRotation ROOMEFFECT_LIFTED_SPARK_ROTATION;
extern u8 ROOMEFFECT_LIFTED_SPARK_COLOR_TABLE[];

/* Spark that is pushed upward each frame, bounces off the floor height and
 * draws as a fading sprite or a spinning coloured one. */
int ROOMEFFECT_LIFTED_SPARK_FUNC(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = ROOMEFFECT_LIFTED_SPARK_ROTATION;
    RenderColor color;
    int scale;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy + 5;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 255 / 256;
            spark->vz = spark->vz * 255 / 256;
            fall = (u16)spark->vy + 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 24) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            int intensity;
            intensity = func_80077DC4((s16)spark->timer << 6) / 32;
            scale = func_80077DC4((s16)spark->timer << 6) / 2;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale, 0xA0, clut, 1,
                          intensity, 0);
            break;
        }
        case 1: {
            int clut;
            int kind;
            int palette;
            scale = func_80077CF4(((s16)spark->timer << 10) / 24) * 2 + 0x2000;
            func_80077DC4(((s16)spark->timer * 1204) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            rotation.z = D_800E27EC << 5;
            func_800CF3AC(ROOMEFFECT_LIFTED_SPARK_COLOR_TABLE, &color,
                          (s16)spark->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 4) + 0x20,
                          clut, 1, 0xA0, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_LIFTED_SPARK_FUNC
#undef ROOMEFFECT_LIFTED_SPARK_ROTATION
#undef ROOMEFFECT_LIFTED_SPARK_COLOR_TABLE
