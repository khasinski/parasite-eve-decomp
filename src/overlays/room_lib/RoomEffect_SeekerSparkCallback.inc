#include "pe1/room_spark.h"
#include "pe1/gte.h"
#ifndef ROOMEFFECT_SEEKER_SPARK_FUNC
#error "ROOMEFFECT_SEEKER_SPARK_FUNC must name the room entry point"
#endif

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomSeekerObject {
    u32 flags;
} RoomSeekerObject;

typedef struct RoomSeekerPool {
    RoomSeekerObject *object;
} RoomSeekerPool;

typedef struct RoomSeekerActor {
    u8 reserved[0x4C];
    u32 flags;                    /* 0x4C */
} RoomSeekerActor;

typedef struct RoomSeekerBattleEntity {
    RoomSeekerActor *actor;
} RoomSeekerBattleEntity;

/* Seeker spawner parameters: the step length along the heading. */
typedef struct RoomSeekerSparkParams {
    s16 distance;
} RoomSeekerSparkParams;

extern RenderColor D_8018F1CC;
extern RenderColor D_8018F1D0;
extern RoomSeekerBattleEntity *D_8009D254;
extern void *D_8009D248;
extern u16 D_8009D1CC;
extern int func_8001CAB0(int x, int z, void *map, int flags);
extern int func_800C6B90(void *position, int radius);

/* Seeker spark: flies along its heading dropping trail sparks, bursts on
 * the player (flagging the battle actor) and dies off the walkable floor;
 * trail sparks drift, slow and bounce on the floor for eight frames. */
int ROOMEFFECT_SEEKER_SPARK_FUNC(int mode, RoomDampedSpark *spark,
                                 RoomSeekerSparkParams *params) {
    GteShortVector delta;
    RenderColor color = D_8018F1CC;
    RenderColor glow = D_8018F1D0;
    RoomDampedSpark *child;
    int intensity;
    int scale;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            func_800CFB7C((GteShortVector *)&spark->vx, params->distance, &delta);
            spark->x += delta.x;
            spark->y += delta.y;
            spark->z += delta.z;
            if ((spark->timer & 3) == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child != 0) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = (func_80071A54() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (func_80071A54() & 0x1F) - 0x10;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            func_800CFB7C((GteShortVector *)&spark->vx, 0xB4, &delta);
            delta.x += spark->x;
            delta.y += spark->y;
            delta.z += spark->z;
            if (func_800C6B90(&delta, 0x32)) {
                if (D_800E2368->active) {
                    RoomSparkChannel *channel = D_800F32D0;
                    if ((((RoomSeekerPool *)channel->pool)->object->flags & 0x3F000000) ==
                        0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomSeekerPool *)channel->pool)->object->flags =
                            (((RoomSeekerPool *)channel->pool)->object->flags & 0xC0FFFFFF) |
                            0x19000000;
                        ((RoomSeekerPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                spark->x = delta.x;
                spark->y = delta.y;
                spark->z = delta.z;
                spark->state = 2;
                spark->timer = 0;
            }
            if (func_8001CAB0(spark->x << 16, spark->z << 16, D_8009D248, D_8009D1CC) == 0) {
                return 1;
            }
            break;
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
            if ((s16)spark->timer >= 8) return 1;
            break;
        case 2:
            spark->timer++;
            if ((s16)spark->timer >= 8) return 1;
            break;
        }
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        switch (spark->state) {
        case 0: {
            int clut;
            int kind;
            int palette;

            intensity = ((D_800E27EC & 1) << 5) + 0x60;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            /* The tpage lookup comes first so its indexed load is
             * scheduled ahead of the parameter stores, as in retail. */
            D_800F3368.tpage = D_800E2850[D_800E11E4[4]];
            D_800F3368.palette = 4;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            /* The kind is re-read through its own label after the call, so
             * it is not addressed off a saved parameter block base. */
            func_800D2370((GteShortVector *)spark, (GteRotation *)&spark->vx, 0x140, 0x78, 0,
                          (D_800F336C == 4 && D_800F3428 != 0) ? 0x60 : 0, 0x40, 0x10,
                          clut, &color, &glow, (s16)intensity, 1);
            if (D_800E27EC & 3) {
                return 0;
            }
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800CFB7C((GteShortVector *)&spark->vx, 0xF0, &delta);
            delta.x += spark->x;
            delta.y += spark->y;
            delta.z += spark->z;
            func_800D004C(&delta, 200, 200, 5, 0, 0x1000, 0x1000, &glow, 0,
                          intensity / 2, 1);
            return 0;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;

            scale = rcos((s16)spark->timer << 7) * 2 / 3;
            intensity = rsin((s16)spark->timer << 7) / 64 + 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter00 = 0x10;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 0x10;
            D_800F3368.extent_y = 0x10;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * (spark->timer & 1) + 0xDC, clut, 1,
                          intensity, &color);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;

            D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0xC0, clut, 1,
                          0x80, &glow);
            break;
        }
        }
        break;
    }
    return 0;
}

#undef ROOMEFFECT_SEEKER_SPARK_FUNC
