#include "pe1/room_spark.h"
#ifndef ROOMEFFECT_COMET_SPARK_FUNC
#error "ROOMEFFECT_COMET_SPARK_FUNC must name the room entry point"
#endif
#ifndef ROOMEFFECT_COMET_SPARK_ROTATION_A
#error "ROOMEFFECT_COMET_SPARK_ROTATION_A must name the first rotation seed"
#endif
#ifndef ROOMEFFECT_COMET_SPARK_ROTATION_B
#error "ROOMEFFECT_COMET_SPARK_ROTATION_B must name the second rotation seed"
#endif
#ifndef ROOMEFFECT_COMET_SPARK_COLOR_TABLE
#error "ROOMEFFECT_COMET_SPARK_COLOR_TABLE must name the colour ramp table"
#endif
#ifndef ROOMEFFECT_COMET_SPARK_TRAIL
#error "ROOMEFFECT_COMET_SPARK_TRAIL must name the trail record (four rotations)"
#endif

extern GteRotation ROOMEFFECT_COMET_SPARK_ROTATION_A;
extern GteRotation ROOMEFFECT_COMET_SPARK_ROTATION_B;
extern u8 ROOMEFFECT_COMET_SPARK_COLOR_TABLE[];

/* Trail record the draw hands to the trail renderer: velocity, drift,
 * steer and position in that order. */
typedef struct RoomCometSparkTrail {
    GteRotation velocity;
    GteRotation drift;
    GteRotation steer;
    GteRotation position;
} RoomCometSparkTrail;

extern RoomCometSparkTrail ROOMEFFECT_COMET_SPARK_TRAIL;

/* Battle actor flags and the primary packet word the burst hit sets. */
typedef struct RoomCometSparkActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomCometSparkActor;

typedef struct RoomCometSparkBattleEntity {
    RoomCometSparkActor *actor;
} RoomCometSparkBattleEntity;

typedef struct RoomCometSparkPacket {
    u32 code;
} RoomCometSparkPacket;

extern RoomCometSparkBattleEntity *D_8009D254;
extern int func_800C6B90(void *position, int radius);
extern void func_800783E4(void *from, void *to, int weightFrom, int weightTo,
                          void *out);
extern void func_800D3114(void *trail, int count, int length, int arg3, int arg4,
                          int arg5, int arg6, int tpage, int clut, int alpha,
                          void *headColor, void *tailColor, int mode);

/* Spark that is born between two wave slots, steers along two random
 * rotations, bounces on the floor and draws as a sprite with a coloured
 * trail. */
int ROOMEFFECT_COMET_SPARK_FUNC(int mode, RoomCometSpark *spark) {
    GteRotation rotationA = ROOMEFFECT_COMET_SPARK_ROTATION_A;
    GteRotation rotationB = ROOMEFFECT_COMET_SPARK_ROTATION_B;
    GteShortVector first;
    GteShortVector second;
    RenderColor color;
    GteShortVector floorPos;
    GteRotation floorSpin;
    GteRotation *spin;
    GteRotation *slot;
    int scale;
    int fall;
    int bounce;

    spin = &rotationA;
    slot = &rotationB;
    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            func_800CE8F0(D_800F32D0->pool, 11, slot, &first);
            func_800CE8F0(D_800F32D0->pool, 15, slot, &second);
            spark->x = (first.x + second.x) / 2;
            spark->y = (first.y + second.y) / 2;
            spark->z = (first.z + second.z) / 2;
            func_800783E4(spark, &spark->vx, 0xAAB, 0x555, &spark->steer);
            func_800783E4(spark, &spark->vx, 0x556, 0xAAA, &spark->drift);
            spark->drift.x += (func_80071A54() & 0x1FF) - 0x100;
            spark->drift.y += (func_80071A54() & 0x1FF) - 0x100;
            spark->drift.z += (func_80071A54() & 0x1FF) - 0x100;
            spark->steer.x += (func_80071A54() & 0xFF) - 0x80;
            spark->steer.y += (func_80071A54() & 0xFF) - 0x80;
            spark->steer.z += (func_80071A54() & 0xFF) - 0x80;
            if ((s16)spark->timer < 8) break;
            return 1;
        case 1:
            spark->timer++;
            if (func_800C6B90(spark, 0x190) && D_800E2368->active) {
                RoomSparkChannel *channel = D_800F32D0;
                RoomCometSparkPacket *packet;
                if (((*(RoomCometSparkPacket **)channel->pool)->code & 0x3F000000) ==
                    0x01000000) {
                    D_8009D254->actor->flags |= 0x4000;
                    packet = *(RoomCometSparkPacket **)channel->pool;
                    packet->code = (packet->code & 0xC0FFFFFF) | 0x21000000;
                    packet = *(RoomCometSparkPacket **)channel->pool;
                    packet->code |= 0x80000000;
                }
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            fall = (u16)spark->vy + 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            {
                u16 clut;
                /* The sprite setup is a do/while (0) block, as from a macro.
                 * cse1 stops at its loop end, so the shared constant 1 only
                 * reaches the draw below in cse2, after mode and the state
                 * value have kept their own registers; cse2 also addresses
                 * the block through a base register (la s0). */
                do {
                    D_800F3368.parameter00 = 0x20;
                    D_800F3368.parameter02 = 2;
                    D_800F3368.extent_x = 0x20;
                    D_800F3368.extent_y = 0x20;
                    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
                    D_800F3368.palette = 3;
                    D_800F3368.parameter06 = 1;
                    D_800F3368.parameter0A = 5;
                    clut = func_80077AA4(0, D_800E1204[D_800F3368.palette] + 2);
                } while (0);
                func_800CEE20((GteShortVector *)spark, 0, 0x800, 0x800,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0x40, clut, 1,
                              0x80, 0);
            }
            {
                u16 clut;
                int kind;
                int palette;
                floorSpin.x = 0x400;
                floorSpin.y = 0;
                floorSpin.z = 0;
                floorSpin.flags = 1;
                floorPos.x = spark->x;
                floorPos.z = spark->z;
                floorPos.y = D_800942EC.count;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                clut = func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                       : palette + 2);
                func_800CEE20(&floorPos, &floorSpin, 0x800, 0x800,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0x40, clut, 3,
                              0x80, 0);
            }
            {
                u16 clut;
                int kind;
                int palette;
                ROOMEFFECT_COMET_SPARK_TRAIL.steer = spark->steer;
                ROOMEFFECT_COMET_SPARK_TRAIL.drift = spark->drift;
                ROOMEFFECT_COMET_SPARK_TRAIL.position = *(GteRotation *)spark;
                ROOMEFFECT_COMET_SPARK_TRAIL.velocity = *(GteRotation *)&spark->vx;
                gte_ldrotmatrix(D_800BCFA4.value);
                gte_ldtransmatrix(D_800BCFA4.value);
                func_800CF3AC(ROOMEFFECT_COMET_SPARK_COLOR_TABLE, &color,
                              (s16)spark->timer * 2);
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                clut = func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                       : palette + 2);
                func_800D3114(&ROOMEFFECT_COMET_SPARK_TRAIL, 3, 0x12C, 0, 0x81, 0x7F,
                              0x1E, D_800E2850[D_800E11E4[11]], clut, 0x80, &color, &color,
                              1);
            }
            break;
        }
        case 1:
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 5;
            scale = func_80077CF4((s16)spark->timer << 6);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800CF3AC(ROOMEFFECT_COMET_SPARK_COLOR_TABLE, &color, (s16)spark->timer);
            func_800D0728((GteShortVector *)spark, 0x1F4, 0x320, 0x18, spin, scale,
                          scale, 0, &color, 0x80, 1);
            if ((s16)spark->timer < 8) {
                u16 clut;
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                clut = func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                       : palette + 2);
                func_800CEE20((GteShortVector *)spark, &rotationA, 0x2000, 0x2000,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0x40, clut, 1,
                              0x80, 0);
            }
            break;
        case 2: {
            u16 clut;
            int half;
            do {
                D_800F3368.parameter00 = 0x10;
                D_800F3368.parameter02 = 1;
                D_800F3368.extent_x = 0x10;
                D_800F3368.extent_y = 0x10;
                D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.parameter0A = 0;
                clut = func_80077AA4(0x50, D_800E1204[D_800F3368.palette]);
            } while (0);
            half = (s16)spark->timer / 2;
            func_800CEE20((GteShortVector *)spark, 0, 0x800, 0x800,
                          (s16)D_800F3368.parameter02 * half + 0xA7, clut, 1, 0x80, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_COMET_SPARK_FUNC
#undef ROOMEFFECT_COMET_SPARK_ROTATION_A
#undef ROOMEFFECT_COMET_SPARK_ROTATION_B
#undef ROOMEFFECT_COMET_SPARK_COLOR_TABLE
#undef ROOMEFFECT_COMET_SPARK_TRAIL
