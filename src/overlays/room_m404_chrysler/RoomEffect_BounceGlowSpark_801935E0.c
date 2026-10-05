#include "pe1/room_m404_effects.h"
#include "pe1/gte.h"

/* Spark of the m404 joint debris burst: state 0 bounces twice off the
 * floor while circling and draws as two glow points, state 1 falls
 * and draws as a spinning sprite tinted by a color track. */
int func_801935E0(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = D_8018F210;
    GteShortVector position;
    RenderColor trackColor;
    RenderColor color = D_8018F218;
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
            spark->vx = spark->vx * 255 / 256;
            spark->vz = spark->vz * 255 / 256;
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
                if (spark->y >= D_800942EC.count) {
                    spark->vy = (s16)bounce / 4;
                }
            }
            spark->z += rsin((s16)spark->timer << 9) / 512;
            spark->x += rcos((s16)spark->timer << 9) / 512;
            if ((s16)spark->timer < 64) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy + 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                int rebound = -(s16)fall;
                spark->vy = rebound;
            }
            if ((s16)spark->timer < 32) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0:
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2104(spark, &color, 40, 0xFF);
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.y += 8;
            func_800D2104(&position, &color, 40, 0xFF);
            break;
        case 1: {
            int clut;
            int kind;
            int palette;
            scale = rcos((s16)spark->timer << 5) + 0x1000;
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            rotation.z = D_800E27EC << 5;
            func_800CF3AC(D_80193F68, &trackColor, (s16)spark->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale / 2, scale / 2,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 4),
                          clut, 1, 0x60, &trackColor);
            break;
        }
        }
        break;
    }
    return 0;
}
