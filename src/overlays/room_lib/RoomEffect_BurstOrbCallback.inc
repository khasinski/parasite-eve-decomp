#include "pe1/room_spark.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_BURST_ORB_FUNC
#error "ROOMEFFECT_BURST_ORB_FUNC must name the room entry point"
#endif

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomBurstOrbObject {
    u32 flags;
} RoomBurstOrbObject;

typedef struct RoomBurstOrbPool {
    RoomBurstOrbObject *object;
} RoomBurstOrbPool;

typedef struct RoomBurstOrbActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomBurstOrbActor;

typedef struct RoomBurstOrbBattleEntity {
    RoomBurstOrbActor *actor;
} RoomBurstOrbBattleEntity;

typedef struct RoomBurstOrbMatrixSlot {
    GteMatrix *value;
} RoomBurstOrbMatrixSlot;

extern GteRotation D_8018EFF4;
extern GteRotation D_8018EFFC;
extern RenderColor D_8018F004;
extern s16 D_80192BFC;
extern RoomBurstOrbBattleEntity *D_8009D254;
extern void *D_8009D248;
extern u16 D_8009D1CC;
extern RoomBurstOrbMatrixSlot g_GeomVramPacketDst;
extern s32 D_800E27EC;
extern u16 D_800E120A;
extern int func_8001CAB0(int x, int z, void *map, int flags);
extern int func_800C6B90(void *position, int radius);

/* Hit the player: flag the battle actor and retarget the primary channel
 * object, as the other room sparks do. */
#define ROOMEFFECT_BURST_ORB_HIT()                                                    \
    if (D_800E2368->active) {                                                         \
        RoomSparkChannel *channel = D_800F32D0;                                       \
        if ((((RoomBurstOrbPool *)channel->pool)->object->flags & 0x3F000000) ==      \
            0x01000000) {                                                             \
            D_8009D254->actor->flags |= 0x4000;                                       \
            ((RoomBurstOrbPool *)channel->pool)->object->flags =                      \
                (((RoomBurstOrbPool *)channel->pool)->object->flags & 0xC0FFFFFF) |   \
                0x19000000;                                                           \
            ((RoomBurstOrbPool *)channel->pool)->object->flags |= 0x80000000;         \
        }                                                                             \
    }

/* Orb that flies along its velocity shedding sparks, bursts into a ring of
 * debris when it reaches the floor and dies off the walkable polygon; the
 * debris states 2..4 fall with different drag and the sprites differ per
 * state. */
int ROOMEFFECT_BURST_ORB_FUNC(int mode, RoomDampedSpark *spark) {
    GteRotation ringRotation = D_8018EFF4;
    GteRotation rotation = D_8018EFFC;
    GteShortVector position;
    RenderColor color = D_8018F004;
    GteShortVector shadow;
    GteRotation shadowRotation;
    RoomDampedSpark *child;
    int i;
    int angle;
    /* Retail reuses these temporaries across states (size: child speed, ring
     * radius, sprite page and frame; fade: the debris sprite size); the
     * sharing decides the s0/s1/s2 assignment. */
    int size;
    int fade;
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
            spark->x += (func_80071A54() & 0x1F) - 0x10;
            spark->y += (func_80071A54() & 0x1F) - 0x10;
            spark->z += (func_80071A54() & 0x1F) - 0x10;
            if (spark->y >= (s16)g_FrameCount16.count) {
                spark->state = 1;
                spark->timer = 0;
                for (i = 0; i < 8; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child != 0) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        size = (func_80071A54() & 0x1F) + 0x64;
                        angle = D_80192BFC + (func_80071A54() & 0x1FF) - 0x100;
                        child->vx = rcos(angle) * size / 4096;
                        child->vz = rsin(angle) * size / 4096;
                        child->vy = -(func_80071A54() & 0x1F) - 0x18;
                        child->state = 4;
                        child->timer = 0;
                    }
                    child = func_800CE610(D_800F33E0->pool);
                    if (child != 0) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        size = (func_80071A54() & 0xF) + 0x30;
                        angle = D_80192BFC + (func_80071A54() & 0x1FF) - 0x100;
                        child->vx = rcos(angle) * size / 4096;
                        child->vz = rsin(angle) * size / 4096;
                        child->vy = -(func_80071A54() & 0xF);
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (spark->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->x += (func_80071A54() & 0x7F) - 0x40;
                    child->y += (func_80071A54() & 0x7F) - 0x40;
                    child->z += (func_80071A54() & 0x7F) - 0x40;
                    child->vx = (func_80071A54() & 0xF) - 8;
                    child->vy = (func_80071A54() & 0xF) - 8;
                    child->vz = (func_80071A54() & 0xF) - 8;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if ((s16)spark->timer % 3 == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = (func_80071A54() & 0x1F) - 0x10;
                    child->vy = (func_80071A54() & 0x1F) - 0x10;
                    child->vz = (func_80071A54() & 0x1F) - 0x10;
                    child->state = 3;
                    child->timer = 0;
                }
            }
            if (spark->timer & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = (func_80071A54() & 0x7F) - 0x40;
                    child->vy = (func_80071A54() & 0x7F) - 0x40;
                    child->vz = (func_80071A54() & 0x7F) - 0x40;
                    child->state = 4;
                    child->timer = 0;
                }
            }
            if (func_800C6B90(spark, 0x96)) {
                ROOMEFFECT_BURST_ORB_HIT();
            }
            if (func_8001CAB0(spark->x << 16, spark->z << 16, D_8009D248, D_8009D1CC)) {
                return 0;
            }
            spark->state = 5;
            spark->timer = 0;
            break;
        case 1:
            spark->timer++;
            if (func_800C6B90(spark, 0x190) && (s16)spark->timer < 9) {
                ROOMEFFECT_BURST_ORB_HIT();
            }
            if ((s16)spark->timer < 16) break;
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
            if (spark->y >= (s16)g_FrameCount16.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 3:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 511 / 512;
            spark->vz = spark->vz * 511 / 512;
            fall = (u16)spark->vy - 3;
            spark->vy = fall;
            if (spark->y >= (s16)g_FrameCount16.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 24) break;
            return 1;
        case 4:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 61 / 62;
            spark->vz = spark->vz * 61 / 62;
            spark->vy += 4;
            if (spark->y >= (s16)g_FrameCount16.count) {
                bounce = -spark->vy;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 5:
            if ((s16)++spark->timer < 6) break;
            return 1;
        }
        break;
    case 2:
        gte_ldrotmatrix(g_GeomVramPacketDst.value);
        gte_ldtransmatrix(g_GeomVramPacketDst.value);
        switch (spark->state) {
        case 0:
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            rotation.z = D_800E27EC << 8;
            {
                u16 clut;
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800CEE20((GteShortVector *)spark, &rotation, 0x1800, 0x1800, 0xA0, clut, 1,
                              0x80, 0);
            }
            fade = ((D_800E27EC & 1) << 5) + 0x80;
            func_800D004C((GteShortVector *)spark, 200, 200, 8, 0, 0x3000, 0x3000, &color, 0,
                          fade, 1);
            shadowRotation.x = 0x400;
            shadowRotation.y = 0;
            shadowRotation.z = 0;
            shadowRotation.flags = 1;
            shadow.x = spark->x;
            shadow.z = spark->z;
            shadow.y = g_FrameCount16.count;
            {
                u16 clut;
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0, palette);
                func_800CEE20(&shadow, &shadowRotation, 0x3000, 0x3000, 0, clut, 3, 0x50, 0);
            }
            return 0;
        case 1:
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            scale = rsin((s16)spark->timer << 6) + 0x1000;
            fade = rcos((s16)spark->timer << 6) / 64;
            ringRotation.z = D_80192BFC + 0x400;
            for (i = 0; i < 3; i++) {
                u16 clut;
                int kind;
                int palette;

                position.x = spark->x;
                position.y = spark->y;
                position.z = spark->z;
                size = (((s16)spark->timer * i) << 4) + 0xC8;
                position.x += rcos(D_80192BFC) * size / 4096;
                position.z += rsin(D_80192BFC) * size / 4096;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                size = 1;
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x20, palette);
                func_800CEE20(&position, &ringRotation, scale, scale, 0x6C, clut, size, fade, 0);
            }
            return 0;
        case 2: {
            int clut;
            int kind;
            int palette;

            scale = rsin(((s16)spark->timer << 10) / 24) / 2 + 0x1000;
            size = (s16)spark->timer;
            if (size >= 8) size += 8;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale * 9 / 4, scale * 9 / 4,
                          (s16)D_800F3368.parameter02 * size + 0x20, clut, 1, 0x80, 0);
            return 0;
        }
        case 3: {
            int clut;
            int kind;
            int palette;

            scale = rsin(((s16)spark->timer << 10) / 24) / 2 + 0x1000;
            fade = rcos((s16)spark->timer * 1204 / 24) / 128 + 0x28;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale * 3, scale * 3,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 6) + 0x80,
                          clut, 1, fade, 0);
            break;
        }
        case 4:
            fade = rcos((s16)spark->timer << 6) / 32 + 0x40;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800D1DEC(spark, &color, fade, 1);
            return 0;
        case 5: {
            u16 clut;
            int half;

            D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 5;
            D_800F3368.palette = 3;
            D_800F3368.depth = 0x20;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            clut = func_80077AA4(0, D_800E1204[3] + 2);
            /* Empty do/while (0), as left by a macro: cse1 stops here, so
             * cse2 addresses the restore below through saved base registers
             * (&D_800F3368.tpage, &D_800E11E4[11]) as retail does. */
            do {
            } while (0);
            half = (s16)spark->timer / 2;
            func_800CEE20((GteShortVector *)spark, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * half + 0x60, clut, 1,
                          0x50, 0);
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_BURST_ORB_FUNC
#undef ROOMEFFECT_BURST_ORB_HIT
