#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_JITTER_SPARK_FUNC
#error "ROOMEFFECT_JITTER_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_JITTER_SPARK_ROTATION
#error "ROOMEFFECT_JITTER_SPARK_ROTATION must name the rotation seed"
#endif

extern GteRotation ROOMEFFECT_JITTER_SPARK_ROTATION;

/* Spark that jitters its position (state 0) or its velocity (states 1 and
 * 2) by a small random amount each frame, bounces off the floor height and
 * draws as a spinning sprite whose size follows a cosine of its age. */
int ROOMEFFECT_JITTER_SPARK_FUNC(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = ROOMEFFECT_JITTER_SPARK_ROTATION;
    /* Retail reserves one 16-byte local that nothing in the shipped code
     * reads; keep it so the frame matches. */
    GteVector unusedVector;
    int angle;
    int size;
    int fade;
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
            fall = (u16)spark->vy;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->x += (func_80071A54() & 0xF) - 7;
            spark->y += (func_80071A54() & 0xF) - 7;
            spark->z += (func_80071A54() & 0xF) - 7;
            if ((s16)spark->timer < 32) break;
            return 1;
        case 1:
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (func_80071A54() & 0xF) - 7;
            spark->vy += (func_80071A54() & 0xF) - 7;
            spark->vz += (func_80071A54() & 0xF) - 7;
            if ((s16)spark->timer < 20) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = (s16)spark->timer << 5;
            fade = rcos(angle) / 60;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 3;
            if (spark->angle & 1) size /= 2;
            rotation.z = spark->angle + (s16)spark->timer * 8;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 0x80,
                          clut, 1, fade, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = ((s16)spark->timer << 10) / 20;
            fade = rcos(angle) / 60;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 2;
            if (spark->angle & 1) size >>= 1;
            rotation.z = spark->angle + (s16)spark->timer * 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 6) + 0x80,
                          clut, 1, fade / 2, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = ((s16)spark->timer << 10) / 20;
            fade = rcos(angle) / 32;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 2;
            if (spark->angle & 1) size >>= 1;
            rotation.z = spark->angle + (s16)spark->timer * 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 6) + 0x80,
                          clut, 2, fade / 4, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_JITTER_SPARK_FUNC
#undef ROOMEFFECT_JITTER_SPARK_ROTATION
