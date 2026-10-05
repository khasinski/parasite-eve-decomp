#include "pe1/room_m349_effects.h"

/* Spark that jitters its velocity (states 0 and 1) or its position
 * (state 2) by a small random amount each frame and bounces off the floor
 * height; draws as a spinning flare, the last state with a glow ring. */
int func_8018F010(int mode, RoomM349FlareSpark *spark) {
    GteRotation rotation = D_8018EFF4;
    RenderColor color = D_8018EFFC;
    int angle;
    int size;
    int fade;
    int frame;
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
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (func_80071A54() & 0xF) - 7;
            spark->vy += (func_80071A54() & 0xF) - 7;
            spark->vz += (func_80071A54() & 0xF) - 7;
            if ((s16)spark->timer < 48) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (func_80071A54() & 7) - 3;
            spark->vy += (func_80071A54() & 7) - 3;
            spark->vz += (func_80071A54() & 7) - 3;
            if ((s16)spark->timer < 32) break;
            return 1;
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
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 48;
            fade = rsin(angle * 2) / 32;
            size = rcos(angle) / 2 + 0x2000;
            rotation.z = spark->angle - (s16)spark->timer * 8;
            frame = ((s16)spark->timer / 2) & 7;
            if (frame >= 4) frame += 12;
            D_800F3368.depth = 0x226;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * frame, clut, 2,
                          fade / 3, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            angle = (s16)spark->timer << 5;
            fade = rsin(angle * 2) / 32;
            size = rcos(angle) / 4 + 0x2000;
            rotation.z = spark->angle + (s16)spark->timer * 12;
            frame = ((s16)spark->timer / 2) & 7;
            if (frame >= 4) frame += 12;
            D_800F3368.depth = 0x80;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * frame, clut, 3, fade, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            fade = rcos((s16)spark->timer << 5) / 40;
            rotation.z = spark->angle;
            size = spark->size;
            D_800F3368.depth = 0x80;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size * 2,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x80,
                          clut, 1, 0x50, 0);
            func_800D004C((GteShortVector *)spark, 500, 500, 8, 0, size, size,
                          &color, 0, fade, 3);
            break;
        }
        }
        break;
    }
    return 0;
}
