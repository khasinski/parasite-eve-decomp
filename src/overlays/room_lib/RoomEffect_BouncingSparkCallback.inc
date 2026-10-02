#include "pe1/room_spark.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_BOUNCING_SPARK_FUNC
#error "ROOMEFFECT_BOUNCING_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_BOUNCING_SPARK_ROTATION
#error "ROOMEFFECT_BOUNCING_SPARK_ROTATION must name the rotation seed"
#endif
#ifndef ROOMEFFECT_BOUNCING_SPARK_SHADE
#error "ROOMEFFECT_BOUNCING_SPARK_SHADE must name the shade colour"
#endif
#ifndef ROOMEFFECT_BOUNCING_SPARK_COLOR_TABLE
#error "ROOMEFFECT_BOUNCING_SPARK_COLOR_TABLE must name the colour ramp table"
#endif

extern GteRotation ROOMEFFECT_BOUNCING_SPARK_ROTATION;
extern RenderColor ROOMEFFECT_BOUNCING_SPARK_SHADE;
extern u8 ROOMEFFECT_BOUNCING_SPARK_COLOR_TABLE[];

/* Spark that rises against gravity, falls once it passes the floor height,
 * then settles; drawn as a spinning sprite, a shaded blob and a flattened
 * sprite in its three states. */
int ROOMEFFECT_BOUNCING_SPARK_FUNC(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = ROOMEFFECT_BOUNCING_SPARK_ROTATION;
    RenderColor color;
    RenderColor shade = ROOMEFFECT_BOUNCING_SPARK_SHADE;
    RenderMatrixSlot *matrixSlot;
    int scale;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            spark->vy += 2;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            spark->vy += 3;
            if (spark->y >= D_800942EC.count) {
                spark->state = 2;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            spark->vy += -1;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            scale = func_80077DC4((D_800E27EC << 10) / 24) + 0x1000;
            rotation.z = D_800E27EC * 50;
            func_800CF3AC(ROOMEFFECT_BOUNCING_SPARK_COLOR_TABLE, &color,
                          (s16)spark->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale, scale,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 3) + 0x20,
                          clut, 1, 0x80, &color);
            break;
        }
        case 1:
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2104(spark, &shade, 0x80, 0xFF);
            break;
        case 2: {
            u16 clut;
            int kind;
            int palette;
            int intensity;
            scale = func_80077CF4(((s16)spark->timer << 10) / 24) + 0x1000;
            intensity = func_80077DC4(((s16)spark->timer * 1204) / 24) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x20,
                          clut, 1, intensity, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_BOUNCING_SPARK_FUNC
#undef ROOMEFFECT_BOUNCING_SPARK_ROTATION
#undef ROOMEFFECT_BOUNCING_SPARK_SHADE
#undef ROOMEFFECT_BOUNCING_SPARK_COLOR_TABLE
