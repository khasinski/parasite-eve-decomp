/* MASPSX_FLAGS: --expand-div */
/*
 * Spark attacks: burst orbs thrown from the actor's hand, a spray of damped
 * sparks, a beam that showers bouncing sparks and a ring of jittering
 * sparks, each a controller and its particle callback.
 *
 * room_m156+2, room_m291+2 and room_m380+2 link these eight functions in
 * this order, with the same 0x64 bytes of read-only seeds and jump tables;
 * this unit is that object, compiled into each of them. The orb heading and
 * the beam's model asset live in each room's own data. The beam's bouncing
 * spark is the RoomLib_UpdateBounceRender template, which the line burst
 * instantiates with other constants.
 */
#include "pe1/room_spark_attacks.h"
#include "pe1/room_line_burst.h"
#include "pe1/room_sound_slot.h"
#include "pe1/render_prim.h"
#include "pe1/gte.h"

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomBurstOrbObject {
    u32 flags;
} RoomBurstOrbObject;

typedef struct RoomBurstOrbPool {
    RoomBurstOrbObject *object;
} RoomBurstOrbPool;

static const GteRotation s_BurstOrbRingRotation = { 0x400, 0, 0, 1 };
static const GteRotation s_BurstOrbRotation = { 0, 0, 0, 0 };
static const RenderColor s_BurstOrbColor = { 0xC8, 0x46, 0x08, 0 };

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
int RoomEffect_BurstOrbParticle(int mode, RoomDampedSpark *spark) {
    GteRotation ringRotation = s_BurstOrbRingRotation;
    GteRotation rotation = s_BurstOrbRotation;
    GteShortVector position;
    RenderColor color = s_BurstOrbColor;
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
            if (spark->y >= (s16)g_RoomFloorY->raw) {
                spark->state = 1;
                spark->timer = 0;
                for (i = 0; i < 8; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child != 0) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        size = (func_80071A54() & 0x1F) + 0x64;
                        angle = g_RoomBurstOrbHeading + (func_80071A54() & 0x1FF) - 0x100;
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
                        angle = g_RoomBurstOrbHeading + (func_80071A54() & 0x1FF) - 0x100;
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
            if (spark->y >= (s16)g_RoomFloorY->raw) {
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
            if (spark->y >= (s16)g_RoomFloorY->raw) {
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
            if (spark->y >= (s16)g_RoomFloorY->raw) {
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
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
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
            shadow.y = g_RoomFloorY->raw;
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
            ringRotation.z = g_RoomBurstOrbHeading + 0x400;
            for (i = 0; i < 3; i++) {
                u16 clut;
                int kind;
                int palette;

                position.x = spark->x;
                position.y = spark->y;
                position.z = spark->z;
                size = (((s16)spark->timer * i) << 4) + 0xC8;
                position.x += rcos(g_RoomBurstOrbHeading) * size / 4096;
                position.z += rsin(g_RoomBurstOrbHeading) * size / 4096;
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
            /* The palette is read back through the parameter block, as the
             * other draws do: cse then keeps &D_800F3368.tpage and
             * &D_800E11E4[11] in saved registers and addresses the restore
             * below off them, as retail does. */
            {
                int kind;
                int palette;

                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 6;
                else palette += 2;
                clut = func_80077AA4(0, palette);
            }
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

static const RoomSparkWords8 s_SparkLaunchOffset = { { 0xFFBA0000, 0x0000FF9C } };

/* Throws a burst orb from the actor's hand (joint 7) along the actor's
 * heading on the first frame, with a fading flash at the hand. */
int RoomEffect_BurstOrbController(int mode, void *unused, s32 *state) {
    RoomSparkWords8 template = s_SparkLaunchOffset;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomDampedSpark *child;
    if (mode == 1)
        goto update;
    if (mode < 2) {
        if (mode == 0)
            goto init;
        return 0;
    }
    if (mode == 2)
        goto configure;
    return 0;
init:
    if (D_800E2368->active) {
        RoomSparkNode **slot = (RoomSparkNode **)D_800F32D0->pool;
        if (slot && *slot) {
            u8 *flag = (*slot)->state;
            if (*flag == 1) *flag = 2;
        }
    }
    pool = D_800F33E0->pool;
    return func_800CE560(pool, 20, 46,
                         (FieldAnimCallbackListCallback)RoomEffect_BurstOrbParticle);
update:
    pool = D_800F32D0->pool;
    func_800CE8F0(pool, 7, &template, position);
    pool = D_800F32D0->pool;
    func_800CE9D4((struct RoomFxTransformOwner *)pool, 0,
                  (GteShortVector *)target);
    g_RoomBurstOrbHeading = -(u16)target[1] + 0x400;
    if (D_800E27EC == 1) {
        pool = D_800F33E0->pool;
        child = func_800CE610(pool);
        if (child) {
            int radius = *state;
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            child->vx = rcos(g_RoomBurstOrbHeading) * radius / 4096;
            child->vz = rsin(g_RoomBurstOrbHeading) * radius / 4096;
            child->vy = 24;
            child->state = 0;
            child->timer = 0;
        }
        pool = D_800F33E0->pool;
        child = func_800CE610(pool);
        if (child) {
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            child->state = 5;
            child->timer = 0;
        }
    }
    if (D_800E27EC < 2) goto done;
    return 2;
configure:
    {
        int palette = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
done:
    return 0;
}

/* Spark particle that drifts with damped velocity, falls under gravity
 * and bounces off the floor height, then draws as a growing sprite. */
int RoomEffect_DampedSparkParticle(int mode, RoomDampedSpark *spark) {
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
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
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
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
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
            scale = (s16)spark->timer;
            if (scale >= 8) scale += 8;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0xC00, 0xC00,
                          (s16)D_800F3368.parameter02 * scale + 0x20, clut, 1,
                          0x80, 0);
            break;
        }
        case 1: {
            int clut;
            int kind;
            int palette;
            scale = rsin(((s16)spark->timer << 10) / 24) + 0x1000;
            scale /= 2;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x30, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 4) + 0x80,
                          clut, 1, 0x60, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Plays the spray sound, then sprays damped sparks from the actor's hand
 * around its heading for 47 frames. */
int RoomEffect_DampedSparkController(int mode, void *unused, void *state) {
    RoomSparkWords8 template = s_SparkLaunchOffset;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomDampedSpark *child;
    int angle, radius;
    if (mode == 1)
        goto update;
    if (mode < 2) {
        if (mode == 0)
            goto init;
        return 0;
    }
    if (mode == 2)
        goto configure;
    return 0;
init:
    {
        int handle = func_800D3FD8();
        func_800D3F64(0x58B, handle);
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 16,
                             (FieldAnimCallbackListCallback)RoomEffect_DampedSparkParticle);
    }
update:
    pool = D_800F32D0->pool;
    func_800CE8F0(pool, 7, &template, position);
    pool = D_800F32D0->pool;
    func_800CE9D4((struct RoomFxTransformOwner *)pool, 0,
                  (GteShortVector *)target);
    angle = -target[1] + 0x400;
    if (D_800E27EC < 47) {
        if (D_800E27EC & 1) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                radius = (func_80071A54() & 15) + 8;
                angle = angle + (func_80071A54() & 0x1FF) - 0x100;
                child->vx = rcos(angle) * radius / 4096;
                child->vz = rsin(angle) * radius / 4096;
                child->vy = -(func_80071A54() & 7);
                child->state = 0;
                child->timer = 0;
            }
        } else {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                radius = (func_80071A54() & 7) + 4;
                angle = angle + (func_80071A54() & 0x1FF) - 0x100;
                child->vx = rcos(angle) * radius / 4096;
                child->vz = rsin(angle) * radius / 4096;
                child->vy = -(func_80071A54() & 15);
                child->state = 1;
                child->timer = 0;
            }
        }
    }
    if (D_800E27EC < 2) goto done;
    return 2;
configure:
    {
        int idx = D_800E11EA;
        int palette;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E2850[idx];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
done:
    return 0;
}

static const RoomLineBurstColor s_BeamBounceTileColor = { 0x80, 0x80, 0x80, 0 };

#define ROOMLIB_UPDATE_BOUNCE_RENDER_NAME RoomEffect_BeamBounceParticle
#define ROOMLIB_BOUNCE_RENDER_BLOB s_BeamBounceTileColor
#define ROOMLIB_BOUNCE_GRAVITY 3
#define ROOMLIB_BOUNCE_CLUT_X 0x30
#define ROOMLIB_BOUNCE_FRAME_BASE 0x80
#include "RoomLib_UpdateBounceRender.inc"

/* Flat-colour GPU line packet with its tag split into address and length. */
typedef struct RoomBeamLinePacket {
    u8 address[3], length;
    u8 r, g, b, code;
    s16 x0, y0, x1, y1;
} RoomBeamLinePacket;

typedef struct RoomBeamSparkMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} RoomBeamSparkMatrix;

/* First object of the primary channel's pool: its packet flags and the
 * actor state byte it owns. */
typedef struct RoomBeamSparkObject {
    u32 flags;
    u8 reserved[0x14];
    u8 *status;                   /* 0x18 */
} RoomBeamSparkObject;

typedef struct RoomBeamSparkPool {
    RoomBeamSparkObject *object;
} RoomBeamSparkPool;

static const GteRotation s_BeamSparkRotation = { 0, 0, -100, 0 };


/* Controller that fires a beam from the actor to the target: it blends the
 * beam tip towards the target while spraying sparks, holds the beam while
 * a scaled model pulses at the target and hits anything in reach, then
 * fades the model out. */
int RoomEffect_BeamSparkController(int mode, RoomBeamSpark *fx,
                               RoomBeamSparkParams *params) {
    GteRotation rotation = s_BeamSparkRotation;
    GteRotation modelRotation = s_BurstOrbRotation;
    GteShortVector actor;
    int depth;
    int scratch;
    RoomBeamLinePacket *line;
    RoomBounceSpark *child;
    RoomBeamSparkPool *pool;
    RoomBeamSparkObject *object;
    RoomSparkChannel *channel;
    RenderMatrixSlot *matrixSlot;
    u16 *tpages;
    u16 *index;
    int angle;
    int speed;
    int handle;
    int i;

    switch (mode) {
    case 0:
        fx->state = 0;
        fx->timer = 0;
        fx->scale = 0;
        fx->glow = 0;
        fx->countdown = 0;
        fx->tx = params->x;
        fx->ty = params->y;
        fx->tz = params->z;
        fx->ty = g_RoomFloorY->raw;
        g_RoomBeamSparkAsset = func_8006E498(D_800B0E64_slot.channel, 0xC5485704);
        func_800C6D5C(g_RoomBeamSparkAsset, 0, 0);
        handle = func_800D3FD8();
        func_800D3F64(0x58A, handle);
        if (D_800E2368->active) {
            pool = (RoomBeamSparkPool *)D_800F32D0->pool;
            if (pool) {
                object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x14, 0x20,
                             (FieldAnimCallbackListCallback)RoomEffect_BeamBounceParticle);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 0x1F, &rotation, fx);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0,
                      &actor);
        angle = -actor.y + 0x400;
        switch (fx->state) {
        case 0:
            fx->timer++;
            if ((s16)fx->timer == 1) {
                for (i = 0; i < 2; i++) {
                    child = func_800CE610(D_800F33E0->pool);
                    if (child) {
                        child->x = fx->x;
                        child->y = fx->y;
                        child->z = fx->z;
                        child->vx = rcos(angle) / 128;
                        child->vz = rsin(angle) / 128;
                        child->vy = 0;
                        child->state = i;
                        child->timer = 0;
                    }
                }
            }
            scratch = 0x1000 - rcos(((s16)fx->timer << 10) / 6);
            LoadAverageShort12(fx, &fx->tx, 0x1000 - scratch, scratch, &fx->tipX);
            fx->brightness = 0x80 - ((s16)fx->timer << 5) / 6;
            if ((s16)fx->timer < 6) break;
            fx->state = 1;
            fx->timer = 0;
            for (i = 0; i < 8; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = fx->tx;
                    child->y = fx->ty;
                    child->z = fx->tz;
                    speed = (func_80071A54() & 0x1F) + 0x20;
                    child->vx = rcos(angle) * speed / 4096;
                    child->vz = rsin(angle) * speed / 4096;
                    child->vy = 0;
                    child->state = 0;
                    child->timer = 0;
                }
                angle += 0x200;
            }
            if (func_8001CAB0(fx->tx << 16, fx->tz << 16, D_8009D248, D_8009D1CC)) break;
            return 1;
        case 1:
            fx->timer++;
            fx->brightness = rcos(((s16)fx->timer << 10) / 6) / 128 + 0x40;
            fx->tipX = fx->tx;
            fx->tipY = fx->ty;
            fx->tipZ = fx->tz;
            fx->scale = rsin(((s16)fx->timer << 10) / 6) + 0x800;
            fx->glow = 0x80;
            if ((s16)fx->timer < 6) break;
            fx->state = 2;
            fx->timer = 0;
            fx->brightness = 0;
            for (i = 0; i < 0x16; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    scratch = func_80071A54() & 0xFFF;
                    LoadAverageShort12(fx, &fx->tx, 0x1000 - scratch, scratch, child);
                    child->vy = 0;
                    child->ay = (func_80071A54() & 3) + 1;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            break;
        case 2:
            fx->timer++;
            if (fx->scale > 0x1000) fx->scale -= 0x400;
            fx->glow = rsin(((s16)fx->timer << 14) / params->duration) / 128 + 0x80;
            if (fx->countdown != 0) fx->countdown--;
            if (func_800C6B90(&fx->tx, 0x258) && fx->countdown == 0) {
                if (D_800E2368->active) {
                    channel = D_800F32D0;
                    if ((((RoomBeamSparkPool *)channel->pool)->object->flags & 0x3F000000) ==
                        0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        ((RoomBeamSparkPool *)channel->pool)->object->flags =
                            (((RoomBeamSparkPool *)channel->pool)->object->flags & 0xC0FFFFFF) |
                            0x19000000;
                        ((RoomBeamSparkPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                fx->countdown = params->interval;
            }
            if ((s16)fx->timer < params->duration) break;
            fx->state = 3;
            fx->timer = 0;
            break;
        case 3:
            fx->timer++;
            fx->scale = rcos((s16)fx->timer << 5);
            fx->glow = rcos((s16)fx->timer << 5) / 32;
            if ((s16)fx->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        if (fx->brightness != 0) {
            line = (RoomBeamLinePacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += 0x10;
            gte_ldv3(fx, &fx->tipX, fx);
            gte_rtpt_padded();
            line->length = 3;
            line->code = 0x40;
            line->r = fx->brightness;
            line->g = fx->brightness;
            line->b = fx->brightness;
            gte_stszotz(&depth);
            if ((u32)(depth - 1) < 0xFFF) {
                gte_stsxy3(&line->x0, &line->x1, &scratch);
                func_800CF6F8(D_800B0E38.ordering[D_8009CDDC] + depth, line, 1);
            }
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        index = &D_800E11EA;
        tpages = D_800E2850;
        {
            int tpage = tpages[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 5;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        if ((s16)fx->glow != 0) {
            RoomBeamSparkMatrix matrix;
            GteVector scale;
            u16 clut;
            int kind;
            int palette;
            int page;
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            {
                int tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            page = (tpages[*index] | func_80077A64(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800C6EC0(page, clut);
            func_800C6ED8(1);
            RotMatrixYXZ(&modelRotation, &matrix);
            matrix.t[0] = fx->tx;
            matrix.t[1] = fx->ty;
            matrix.t[2] = fx->tz;
            scale.x = fx->scale * 3 / 2;
            scale.y = fx->scale * 3 / 2;
            scale.z = fx->scale * 3 / 2;
            ScaleMatrix(&matrix, &scale);
            func_800C6EF8(g_RoomBeamSparkAsset);
            func_800C6FA0(g_RoomBeamSparkAsset, (u16)((s16)fx->glow / 2));
            func_800C71E4(g_RoomBeamSparkAsset, &matrix);
            func_800C6F4C(g_RoomBeamSparkAsset);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

static const GteRotation s_JitterSparkRotation = { 0, 0, 0, 0 };

/* Spark that jitters its position (state 0) or its velocity (states 1 and
 * 2) by a small random amount each frame, bounces off the floor height and
 * draws as a spinning sprite whose size follows a cosine of its age. */
int RoomEffect_JitterSparkParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = s_JitterSparkRotation;
    /* Retail reserves one 16-byte local that nothing in the shipped code
     * reads; keep it so the frame matches. */
    GteVector unusedVector;
    int angle;
    int size;
    int fade;
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
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->x += (func_80071A54() & 0xF) - 7;
            spark->y += (func_80071A54() & 0xF) - 7;
            spark->z += (func_80071A54() & 0xF) - 7;
            if ((s16)spark->timer < 32) break;
            return 1;
        case 1:
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (func_80071A54() & 0xF) - 7;
            spark->vy += (func_80071A54() & 0xF) - 7;
            spark->vz += (func_80071A54() & 0xF) - 7;
            if ((s16)spark->timer < 20) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = (s16)spark->timer << 5;
            fade = rcos(angle) / 60;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 3;
            if (spark->angle & 1) size /= 2;
            rotation.z = spark->angle + (s16)spark->timer * 8;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 0x80,
                          clut, 1, fade, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = ((s16)spark->timer << 10) / 20;
            fade = rcos(angle) / 60;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 2;
            if (spark->angle & 1) size >>= 1;
            rotation.z = spark->angle + (s16)spark->timer * 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 6) + 0x80,
                          clut, 1, fade / 2, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            int scale;
            angle = ((s16)spark->timer << 10) / 20;
            fade = rcos(angle) / 32;
            scale = rcos(angle) / 4 + 0x1000;
            size = scale * 2;
            if (spark->angle & 1) size >>= 1;
            rotation.z = spark->angle + (s16)spark->timer * 16;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 6) + 0x80,
                          clut, 2, fade / 4, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Controller that bursts a ring of jittering sparks on the first frame,
 * keeps spawning pairs for the first 32 frames, plays the two burst sounds
 * on creation and configures the sprite palette. */
int RoomEffect_JitterSparkController(int mode, GteShortVector *target,
                                      RoomSparkRingParams *params) {
    RoomDampedSpark *child;
    int radius;
    int angle;
    int reach;
    int handle;
    RoomSoundSlot *sound;
    int volume;
    int i;

    switch (mode) {
    case 0:
        sound = &D_800B0E64_slot;
        if (sound->channel != 0) {
            volume = 0x7F;
            handle = func_800D3FD8();
            func_8006DF50(sound->channel, 0x5FA, handle, 0x80, volume);
            if (sound->channel != 0) {
                func_8006DF50(sound->channel, 0x5FB, 0x80, 0x80, volume);
            }
        }
        target->x = params->x;
        target->y = params->y;
        target->z = params->z;
        target->y = g_RoomFloorY->raw;
        return func_800CE560(D_800F33E0->pool, 20, 60,
                             (FieldAnimCallbackListCallback)RoomEffect_JitterSparkParticle);
    case 1:
        radius = params->radius;
        if (D_800E27EC == 1) {
            for (i = 0; i < 20; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 64 - 32;
                    child->vy = -(func_80071A54() % 44);
                    child->vz = func_80071A54() % 64 - 32;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            for (i = 0; i < 16; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 128 - 64;
                    child->vy = -(func_80071A54() % 80);
                    child->vz = func_80071A54() % 128 - 64;
                    child->state = (func_80071A54() & 1) + 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 33) {
            if (D_800E27EC & 1) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = target->x;
                    child->y = target->y;
                    child->z = target->z;
                    angle = func_80071A54();
                    reach = radius * (func_80071A54() & 0xFF) / 256;
                    child->x += rcos(angle) * reach / 4096;
                    child->z += rsin(angle) * reach / 4096;
                    child->angle = func_80071A54();
                    child->vx = func_80071A54() % 70 - 35;
                    child->vy = -(func_80071A54() % 44);
                    child->vz = func_80071A54() % 70 - 35;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = target->x;
                child->y = target->y;
                child->z = target->z;
                angle = func_80071A54();
                reach = radius * (func_80071A54() & 0xFF) / 256;
                child->x += rcos(angle) * reach / 4096;
                child->z += rsin(angle) * reach / 4096;
                child->angle = func_80071A54();
                child->vx = func_80071A54() % 80 - 40;
                child->vy = -(func_80071A54() % 49);
                child->vz = func_80071A54() % 80 - 40;
                child->state = (func_80071A54() & 1) + 1;
                child->timer = 0;
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        int index = D_800E11EA;
        int tpage;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        tpage = D_800E2850[index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = tpage;
        break;
    }
    }
    return 0;
}
