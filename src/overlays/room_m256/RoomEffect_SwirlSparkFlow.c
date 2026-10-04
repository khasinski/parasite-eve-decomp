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

/* Swirl spark controller: plays its two sounds on start, then while the
 * effect is young sheds two swirl sparks per frame from the tracked joint,
 * aimed through one of three (then one of two) headings picked by a
 * rotating counter; mode 2 sets the sprite parameters. */
int func_80193BFC(int mode, s16 *counter) {
    RoomM256Template template = D_8018F210;
    GteShortVector position;
    GteShortVector base;
    GteShortVector angles;
    RoomM256SwirlSpark *child;
    void **soundSlot;
    int distance = 0;
    int volume;
    int time;

    switch (mode) {
    case 0:
        *counter = 0;
        soundSlot = &D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            func_8006DF50(*soundSlot, 0x5A7, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                func_8006DF50(*soundSlot, 0x5BF, 0x80, 0x80, volume);
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20, func_8019377C);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 4, &template, &position);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 4, &base);
        if (D_800E27EC < 0x6F) {
            child = (RoomM256SwirlSpark *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter) {
                case 0:
                    distance = (func_80071A54() & 7) + 14;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0x7F) - 0x40;
                    break;
                case 1:
                    distance = (func_80071A54() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0x7F) + 0x3C0;
                    break;
                case 2:
                    distance = (func_80071A54() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (func_80071A54() & 0x7F) + 0x3C0;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
            child = (RoomM256SwirlSpark *)func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter & 1) {
                case 0:
                    distance = (func_80071A54() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (func_80071A54() & 0xFF) + 0x380;
                    break;
                case 1:
                    distance = (func_80071A54() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (func_80071A54() & 0xFF) + 0x380;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->small = func_80071A54() & 1;
                child->state = 1;
                child->timer = 0;
            }
            *counter = (s16)(*counter + 1) % 3;
        }
        if (D_800E27EC < 2) {
            break;
        }
        return 2;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
