#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_JITTER_SIZED_FUNC
#error "ROOMEFFECT_JITTER_SIZED_FUNC must name the particle callback"
#endif
#ifndef ROOMEFFECT_JITTER_SIZED_ROTATION
#error "ROOMEFFECT_JITTER_SIZED_ROTATION must name the initial rotation"
#endif

extern GteRotation ROOMEFFECT_JITTER_SIZED_ROTATION;

/* Spark that drifts with damped velocity, jitters and spins for 8 frames
 * while drawing an animated sprite scaled by its size field. */
int ROOMEFFECT_JITTER_SIZED_FUNC(int mode, RoomJitterSpark *spark) {
    GteRotation rotation = ROOMEFFECT_JITTER_SIZED_ROTATION;
    int fall;
    int bounce;
    int size;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
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
            spark->x += (func_80071A54() & 7) - 3;
            spark->y += (func_80071A54() & 7) - 3;
            spark->z += (func_80071A54() & 7) - 3;
            spark->angle += 8;
            if ((s16)spark->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            rcos((s16)spark->timer << 5);
            rotation.z = spark->angle;
            size = spark->size * 4;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, &rotation, size * 3 / 2, size * 3 / 2,
                          (s16)D_800F3368.parameter02 * (s16)spark->timer + 184,
                          func_80077AA4(208, palette), 1, 88, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_JITTER_SIZED_FUNC
#undef ROOMEFFECT_JITTER_SIZED_ROTATION
