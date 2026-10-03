#include "pe1/room_m256_effects.h"
#include "pe1/gte.h"

/* Spark that jitters sideways (state 0) or swirls on a spinning circle
 * (state 1) while it falls with damped velocity and bounces off the
 * floor height; draws as a fixed or a pulsing sprite. */
int func_8019377C(int mode, RoomM256SwirlSpark *spark) {
    RenderMatrixSlot *matrixSlot;
    int fall;
    int bounce;
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
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->x += (func_80071A54() & 1) * 2 - 1;
            spark->z += (func_80071A54() & 1) * 2 - 1;
            if ((s16)spark->timer < 8) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 511 / 512;
            spark->vz = spark->vz * 511 / 512;
            fall = (u16)spark->vy - 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->spin += 0x100;
            spark->x += rsin(spark->spin << 8) / 1024;
            spark->z += rcos(spark->spin << 8) / 1024;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1800, 0x1800,
                          (s16)D_800F3368.parameter02 * (s16)spark->timer,
                          clut, 1, 100, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int page = 2;
            scale = rsin(((s16)spark->timer << 10) / 24) + 0x1000;
            scale /= 2;
            if (spark->small) page = 1;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x40,
                          clut, page, 0x50, 0);
            break;
        }
        }
        break;
    }
    return 0;
}
