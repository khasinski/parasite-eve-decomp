#include "pe1/room_spark.h"
#include "pe1/gte.h"
#include "pe1/room_rising_spray.h"
#ifndef ROOMEFFECT_RISING_SPRAY_PARTICLE_FUNC
#error "ROOMEFFECT_RISING_SPRAY_PARTICLE_FUNC must name the particle callback"
#endif

extern RoomSprayBattleEntity *D_8009D254;
extern void *D_8009D248;
extern u16 D_8009D1CC;
extern int func_8001CAB0(int x, int z, void *map, int flags);
extern int func_800C6B90(void *position, int radius);

/* Spray spark: rises along a swirling path and knocks the player back when
 * it reaches them, then hovers, or falls and bounces off the floor; drawn
 * as a spinning glow with a floor shadow, a fading glow or a shrinking
 * spark. */
int ROOMEFFECT_RISING_SPRAY_PARTICLE_FUNC(int mode, RoomSpraySpark *spark) {
    GteRotation rotation;
    GteShortVector position;
    GteShortVector shadow;
    GteRotation shadowRotation;
    int fall;
    int bounce;
    int width;
    int height;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->y += func_80077CF4((s16)spark->timer * 292) / 256;
            spark->swirl += 200;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += func_80077DC4(spark->swirl) / 128;
            position.z += func_80077CF4(spark->swirl) / 128;
            if (func_800C6B90(&position, 100)) {
                if (D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if ((((RoomSprayHitPool *)channel->pool)->object->flags & 0x3F000000) == 0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomSprayHitPool *)channel->pool)->object->flags =
                            (((RoomSprayHitPool *)channel->pool)->object->flags & 0xC0FFFFFF) | 0x19000000;
                        ((RoomSprayHitPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                spark->vx = 0;
                spark->vy = 0;
                spark->vz = 0;
                spark->state = 1;
                spark->timer = 0;
            }
            if ((s16)spark->timer >= 0x22) {
                spark->state = 1;
                spark->timer = 0;
                spark->vx /= 2;
                spark->vy /= 2;
                spark->vz /= 2;
            }
            if (func_8001CAB0(spark->x << 16, spark->z << 16, D_8009D248, D_8009D1CC)) break;
            return 1;
        case 1:
            spark->timer++;
            spark->y -= 2;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            if ((s16)spark->timer < 0x10) break;
            return 1;
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (spark->state) {
        case 0: {
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 32;
            rotation.flags = 0;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += func_80077DC4(spark->swirl) / 128;
            position.z += func_80077CF4(spark->swirl) / 128;
            width = func_80077DC4((s16)spark->timer * 384) / 4 + 0xC00;
            width *= 2;
            height = func_80077CF4((s16)spark->timer * 384) / 4 + 0xC00;
            height *= 2;
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x70, palette);
                func_800CEE20(&position, &rotation, width, height, 0xA3, clut, 1,
                              0x80, 0);
            }
            shadowRotation.x = 0x400;
            shadowRotation.y = 0;
            shadowRotation.z = (s16)spark->timer * 80;
            shadowRotation.flags = 1;
            shadow.x = position.x;
            shadow.z = position.z;
            shadow.y = D_800942EC.count;
            {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x70, palette);
                func_800CEE20(&shadow, &shadowRotation, width, height, 0xA0, clut, 2,
                              0x30, 0);
            }
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.x += func_80077DC4(spark->swirl) / 128;
            position.z += func_80077CF4(spark->swirl) / 128;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x70, palette);
            func_800CEE20(&position, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x80,
                          clut, 1, 0x80, 0);
            break;
        }
        case 2: {
            int t = (s16)spark->timer;
            height = t - 16;
            if (t <= 16) {
                u16 clut;
                int kind;
                int palette;
                width = func_80077CF4(t << 6) / 2 + 0x800;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x70, palette);
                func_800CEE20((GteShortVector *)spark, 0, width, width, 0xA3, clut, 1, 0x80, 0);
            } else {
                u16 clut;
                int kind;
                int palette;
                height /= 2;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x70, palette);
                func_800CEE20((GteShortVector *)spark, 0, 0xAAA, 0xAAA,
                              (s16)D_800F3368.parameter02 * height + 0x80,
                              clut, 1, 0x80, 0);
            }
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_RISING_SPRAY_PARTICLE_FUNC
