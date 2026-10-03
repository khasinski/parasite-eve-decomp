#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_PULSE_GLINT_FUNC
#error "ROOMEFFECT_PULSE_GLINT_FUNC must name the particle callback"
#endif

/* Spark that drifts with damped velocity and bounces off the floor
 * height for 24 frames; state 0 draws a spinning glint whose size and
 * fade follow a sine and a cosine of its age, state 1 also spins its
 * seed angle and draws a fixed frame glint that shrinks as it fades. */
int ROOMEFFECT_PULSE_GLINT_FUNC(int mode, RoomDampedSpark *spark) {
    GteRotation rotation;
    int fall;
    int bounce;
    int angle;
    int size;
    int fade;
    int frame;

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
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            spark->angle += 20;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            fall = (u16)spark->vy + 1;
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
            angle = ((s16)spark->timer << 10) / 24;
            size = rsin(angle) + 0x2000;
            fade = rcos(angle) / 80;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = spark->angle + (s16)spark->timer * 32;
            rotation.flags = 0;
            frame = (s16)spark->timer / 8;
            D_800F3368.parameter00 = 16;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 16;
            D_800F3368.extent_y = 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x80, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * frame + 0xFD, clut, 1,
                          fade, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 24;
            fade = rcos(angle) / 46;
            rotation.z = spark->angle;
            size = rcos(angle) / 4;
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 0x98,
                          clut, 1, fade, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_PULSE_GLINT_FUNC
