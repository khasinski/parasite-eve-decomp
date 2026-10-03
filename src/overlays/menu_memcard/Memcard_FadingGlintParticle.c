#include "pe1/room_spark.h"

/* Spark that drifts with damped velocity for 24 frames; state 0 draws a
 * glint whose size follows a sine scaled by the spark's angle field, state 1
 * also falls and bounces and draws a larger spinning glint. */
int Memcard_FadingGlintParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation;
    int fall;
    int bounce;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vy = spark->vy * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
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
            size = rsin(angle) + 0x800;
            size = size * (s16)spark->angle / 4096;
            fade = rcos(angle) / 32 + 40;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 32;
            rotation.flags = 0;
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            clut = func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 6,
                          clut, 1, fade, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 24;
            size = rsin(angle) + 0x1000;
            fade = rsin(angle * 2) / 32;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 14;
            rotation.flags = 0;
            D_800F3368.parameter00 = 64;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 64;
            D_800F3368.extent_y = 64;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            clut = func_80077AA4(0, (kind == 4 && D_800F3428) ? palette + 5 : palette + 1);
            func_800CEE20((GteShortVector *)spark, &rotation, size * 2, size * 2, 0,
                          clut, 1, fade, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
