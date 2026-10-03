#include "pe1/room_phased_spark.h"
#ifndef ROOMEFFECT_PHASED_SPARK_FUNC
#error "ROOMEFFECT_PHASED_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_PHASED_SPARK_ROTATION
#error "ROOMEFFECT_PHASED_SPARK_ROTATION must name the first rotation seed"
#endif
#ifndef ROOMEFFECT_PHASED_SPARK_SPIN
#error "ROOMEFFECT_PHASED_SPARK_SPIN must name the second rotation seed"
#endif

extern GteRotation ROOMEFFECT_PHASED_SPARK_ROTATION;
extern GteRotation ROOMEFFECT_PHASED_SPARK_SPIN;


/* Flashes the scene: marks the battle actor and rewrites the colour code of
 * the first packet in the primary effect channel. */
#define ROOMEFFECT_PHASED_SPARK_FLASH()                                      \
    {                                                                        \
        RoomPhasedSparkChannel *channel = D_800F32D0;                              \
        RoomPhasedSparkPacket *packet;                                             \
        if (((*(RoomPhasedSparkPacket **)channel->pool)->code & 0x3F000000) ==     \
            0x01000000) {                                                    \
            D_8009D254->actor->flags |= 0x4000;                              \
            packet = *(RoomPhasedSparkPacket **)channel->pool;                     \
            packet->code = (packet->code & 0xC0FFFFFF) | 0x19000000;         \
            packet = *(RoomPhasedSparkPacket **)channel->pool;                     \
            packet->code |= 0x80000000;                                      \
        }                                                                    \
    }

/* Spark that drops from above the actor, lands on the floor height, then
 * sheds flashing and drifting children while the scene flashes. */
int ROOMEFFECT_PHASED_SPARK_FUNC(int mode, RoomPhasedSpark *spark) {
    GteRotation rotation = ROOMEFFECT_PHASED_SPARK_ROTATION;
    GteRotation spin = ROOMEFFECT_PHASED_SPARK_SPIN;
    GteShortVector target;
    GteShortVector floorPos;
    GteRotation floorSpin;
    RoomPhasedSpark *child;
    /* Shared across the draw cases: a single-set temporary would be
     * scheduled to the end of its block, retail keeps the quotient early. */
    int scale;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            switch (spark->phase) {
            case 0:
                spark->timer++;
                spark->y -= 0x6E;
                if ((s16)spark->timer >= 16) {
                    spark->phase = 1;
                    spark->timer = 0;
                    func_800CE870(D_8009D254, 0, &target);
                    spark->x = target.x + func_80071A54() % 1200 - 600;
                    spark->z = target.z + func_80071A54() % 1200 - 600;
                }
                break;
            case 1:
                spark->timer++;
                spark->y += 0x6E;
                if (spark->y >= D_800942EC.count) {
                    spark->state = 1;
                    spark->timer = 0;
                    spark->y = D_800942EC.count;
                }
                break;
            }
            if (spark->phase < 2 && (D_800E27EC & 1)) {
                if (func_80071A54() & 1) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->state = 2;
                        child->timer = 0;
                        child->phase = 0;
                    }
                } else {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->vx = (func_80071A54() & 0x1F) - 16;
                        child->vz = (func_80071A54() & 0x1F) - 16;
                        child->vy = (func_80071A54() & 3) - 2;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (func_800C6B90(spark, 0x6E) == 0) break;
            if (spark->y < D_800942EC.count - 0x202) break;
            if (D_800E2368->active == 0) break;
            ROOMEFFECT_PHASED_SPARK_FLASH();
            break;
        case 1:
            spark->timer++;
            if (D_800E27EC & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->x = (func_80071A54() & 0xFF) - 0x80;
                    child->z = (func_80071A54() & 0xFF) - 0x80;
                    child->vx = (func_80071A54() & 0x1F) - 16;
                    child->vz = (func_80071A54() & 0x1F) - 16;
                    child->vy = (func_80071A54() & 3) - 2;
                    child->state = 3;
                    child->timer = 0;
                }
            }
            if (func_800C6B90(spark, 0xC8) && (s16)spark->timer < 9 &&
                D_800E2368->active) {
                ROOMEFFECT_PHASED_SPARK_FLASH();
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 2:
            spark->timer++;
            spark->y += spark->phase;
            spark->phase += 2;
            if ((s16)spark->timer < 8) break;
            return 1;
        case 3:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
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
            scale = (func_80077DC4(D_800E27EC << 10) / 8 + 0x1000) * 3 / 4;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            spin.z = D_800E27EC << 8;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)spark, &spin, scale, scale, 0xA0, clut, 0,
                          0xBE, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            D_800F3368.parameter02 = 1;
            floorSpin.x = 0x400;
            floorSpin.y = 0;
            floorSpin.z = 0;
            floorSpin.flags = 1;
            floorPos.x = spark->x;
            floorPos.z = spark->z;
            floorPos.y = D_800942EC.count;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20(&floorPos, &floorSpin, scale * 2, scale * 2,
                          (s16)D_800F3368.parameter02 * 3 + 0xA7, clut, 2, 0x64, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            scale = func_80077DC4((s16)spark->timer << 6) + 0x1000;
            fade = (s16)spark->timer / 2;
            if (fade >= 7) fade = 6;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale * 3 / 2,
                          scale * 3 / 2, (s16)D_800F3368.parameter02 * fade, clut,
                          0, 0xBE, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            scale = func_80077DC4((s16)spark->timer << 7) / 2 + 0x800;
            fade = func_80077DC4((s16)spark->timer << 7) / 32;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale, 0xA2, clut, 1,
                          fade, 0);
            break;
        }
        case 3: {
            u16 clut;
            int kind;
            int palette;
            scale = func_80077CF4(((s16)spark->timer << 10) / 24) * 2 + 0x1000;
            fade = func_80077DC4(((s16)spark->timer * 1204) / 24) / 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x20,
                          clut, 1, fade, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_PHASED_SPARK_FLASH
#undef ROOMEFFECT_PHASED_SPARK_FUNC
#undef ROOMEFFECT_PHASED_SPARK_ROTATION
#undef ROOMEFFECT_PHASED_SPARK_SPIN
