/*
 * Spark barrage: five spark attacks, each a particle callback and its
 * controller. Phased sparks drop from above the player, land on the floor
 * and flash the scene; bouncing sparks spray from the actor's hand; wave
 * sparks ride the actor's joints and collapse into a burst; comet sparks
 * are fired along random directions in bursts of ten; lifted sparks rise
 * from the actor and bounce on the floor.
 *
 * room_m188 and room_m390 link these ten functions in this order, with the
 * same 0x34 bytes of read-only seeds; this unit is that object, compiled
 * into each of them. The colour ramps and the comet's trail record live in
 * each room's own data.
 */
#include "pe1/room_spark_barrage.h"
#include "pe1/gte.h"

static const GteRotation s_FloorRotation = { 0x400, 0, 0, 1 };
static const GteRotation s_ZeroRotation = { 0, 0, 0, 0 };
static const GteRotation s_HandOffset = { 0, 0, -24, 0 };
static const RenderColor s_BounceShade = { 0xC8, 0x00, 0xC8, 0 };
static const GteRotation s_WaveOffsetA = { 0x6E, 0, 0, 0 };
static const GteRotation s_WaveOffsetB = { -0x6E, 0, 0, 0 };
static const GteRotation s_WaveOffsetC = { 0, 0, -40, 0 };

/* First word of the packet the primary channel's pool points at. */
typedef struct RoomSparkBarragePacket {
    u32 code;
} RoomSparkBarragePacket;

/* Actor object behind the primary channel's pool: its bone matrices. */
typedef struct RoomSparkBarrageOwner {
    u8 reserved[0x238];
    u8 *bone;                     /* 0x238 */
} RoomSparkBarrageOwner;


/* Flashes the scene: marks the battle actor and rewrites the colour code of
 * the first packet in the primary effect channel. */
#define ROOMEFFECT_SPARK_BARRAGE_FLASH()                                      \
    {                                                                        \
        RoomSparkChannel *channel = D_800F32D0;                              \
        RoomSparkBarragePacket *packet;                                             \
        if (((*(RoomSparkBarragePacket **)channel->pool)->code & 0x3F000000) ==     \
            0x01000000) {                                                    \
            D_8009D254->actor->flags |= 0x4000;                              \
            packet = *(RoomSparkBarragePacket **)channel->pool;                     \
            packet->code = (packet->code & 0xC0FFFFFF) | 0x19000000;         \
            packet = *(RoomSparkBarragePacket **)channel->pool;                     \
            packet->code |= 0x80000000;                                      \
        }                                                                    \
    }

/* Spark that drops from above the actor, lands on the floor height, then
 * sheds flashing and drifting children while the scene flashes. */
int RoomEffect_PhasedSparkParticle(int mode, RoomPhasedSpark *spark) {
    GteRotation rotation = s_FloorRotation;
    GteRotation spin = s_ZeroRotation;
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
            ROOMEFFECT_SPARK_BARRAGE_FLASH();
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
                ROOMEFFECT_SPARK_BARRAGE_FLASH();
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
            scale = (rcos(D_800E27EC << 10) / 8 + 0x1000) * 3 / 4;
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
            scale = rcos((s16)spark->timer << 6) + 0x1000;
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
            scale = rcos((s16)spark->timer << 7) / 2 + 0x800;
            fade = rcos((s16)spark->timer << 7) / 32;
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
            scale = rsin(((s16)spark->timer << 10) / 24) * 2 + 0x1000;
            fade = rcos(((s16)spark->timer * 1204) / 24) / 32;
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


/* Phased spark controller: every sixth frame drops one spark at joint 6
 * until the six in the caller's counter are spent. */
int RoomEffect_PhasedSparkController(int mode, s16 *counter)
{
    GteRotation position = s_HandOffset;
    GteShortVector output;
    RoomPhasedSpark *particle;

    switch (mode) {
    case 0: {
        RoomSparkEventState *event = D_800E2368;
        *counter = 6;
        if (event->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)D_800F32D0->pool;
            if (nodes != 0) {
                RoomSparkNode *node = *nodes;
                if (node != 0) {
                    u8 *status = node->state;
                    if (*status == 1) *status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 24, 32, RoomEffect_PhasedSparkParticle);
    }
    case 1:
        func_800CE8F0(D_800F32D0->pool, 6, &position, &output);
        if (D_800E27EC % 6 == 0 && *counter > 0) {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->x = output.x;
                particle->y = output.y;
                particle->z = output.z;
                particle->state = 0;
                particle->phase = 0;
                particle->timer = 0;
            }
            --*counter;
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        u16 tpage = D_800E2850[D_800E11EA];
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


/* Spark that rises against gravity, falls once it passes the floor height,
 * then settles; drawn as a spinning sprite, a shaded blob and a flattened
 * sprite in its three states. */
int RoomEffect_BouncingSparkParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = s_ZeroRotation;
    RenderColor color;
    RenderColor shade = s_BounceShade;
    RenderMatrixSlot *matrixSlot;
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
            spark->vy += 2;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            spark->vy += 3;
            if (spark->y >= D_800942EC.count) {
                spark->state = 2;
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
            spark->vy += -1;
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
            scale = rcos((D_800E27EC << 10) / 24) + 0x1000;
            rotation.z = D_800E27EC * 50;
            func_800CF3AC(g_RoomBouncingSparkColors, &color,
                          (s16)spark->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale, scale,
                          (s16)D_800F3368.parameter02 * (s16)((s16)spark->timer / 3) + 0x20,
                          clut, 1, 0x80, &color);
            break;
        }
        case 1:
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D2104((GteShortVector *)spark, (u8 *)&shade, 0x80, 0xFF);
            break;
        case 2: {
            u16 clut;
            int kind;
            int palette;
            int intensity;
            scale = rsin(((s16)spark->timer << 10) / 24) + 0x1000;
            intensity = rcos(((s16)spark->timer * 1204) / 24) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1000, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x20,
                          clut, 1, intensity, 0);
            break;
        }
        }
        break;
    }
    return 0;
}


/* Bouncing spark controller: plays the sound on frame 8 and sprays one
 * spark per frame from joint 7 for 74 frames, one in three of them faster,
 * rotated by the joint's matrix. */
int RoomEffect_BouncingSparkController(int mode)
{
    GteRotation position = s_HandOffset;
    GteShortVector output;
    s16 motion[4];
    RoomDampedSpark *particle;
    int kind;

    switch (mode) {
    case 0:
        if (D_800E2368->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)D_800F32D0->pool;
            if (nodes != 0) {
                RoomSparkNode *node = *nodes;
                if (node != 0) {
                    u8 *status = node->state;
                    if (*status == 1) *status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 20, 32, RoomEffect_BouncingSparkParticle);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 7, &position, &output);
        if (D_800E27EC == 8)
            func_800D3F64(0x593, func_800D3FD8());
        if (D_800E27EC < 74) {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->x = output.x;
                particle->y = output.y;
                particle->z = output.z;
                motion[0] = (func_80071A54() & 31) - 16;
                motion[1] = (func_80071A54() & 31) - 16;
                motion[2] = -32 - (func_80071A54() & 31);
                kind = func_80071A54() % 3;
                if (kind == mode) {
                    motion[0] = motion[0] * 3 / 2;
                    motion[1] = motion[1] * 3 / 2;
                    motion[2] = motion[2] * 3 / 2;
                }
                func_800CEAE8(((RoomSparkBarrageOwner *)D_800F32D0->pool)->bone + 0xE0,
                              motion, &particle->vx);
                particle->state = kind;
                particle->timer = 0;
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        u16 tpage;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        tpage = D_800E2850[D_800E11EA];
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


/* Small flash drawn at the projected position during the first frames. */
#define ROOMEFFECT_WAVE_SPARK_FLASH()                                        \
    if ((s16)spark->timer < 4) {                                             \
        u16 clut;                                                            \
        int kind;                                                            \
        int palette;                                                         \
        D_800F3368.parameter00 = 0x10;                                       \
        D_800F3368.parameter02 = 1;                                          \
        D_800F3368.extent_x = 0x10;                                          \
        D_800F3368.extent_y = 0x10;                                          \
        kind = D_800F3368.palette;                                           \
        palette = D_800E1204[kind];                                          \
        if (kind == 4 && D_800F3428 != 0) palette += 4;                      \
        clut = func_80077AA4(0x40, palette);                                 \
        func_800CEE20(&position, 0, 0x2000, 0x2000,                          \
                      (s16)D_800F3368.parameter02 * (spark->timer & 3) + 0xA3, \
                      clut, 1, 0x80, 0);                                     \
    }

/* Larger sine-scaled sprite at the projected position. */
#define ROOMEFFECT_WAVE_SPARK_BLOOM(recompute)                               \
    {                                                                        \
        u16 clut;                                                            \
        int kind;                                                            \
        int palette;                                                         \
        D_800F3368.parameter00 = 0x20;                                       \
        D_800F3368.parameter02 = 2;                                          \
        D_800F3368.extent_x = 0x20;                                          \
        D_800F3368.extent_y = 0x20;                                          \
        if (recompute) {                                                     \
            fade = rcos(((s16)spark->timer << 10) / 24) / 32;       \
            scale = rsin(((s16)spark->timer << 10) / 24) + 0x800;   \
        }                                                                    \
        kind = D_800F3368.palette;                                           \
        palette = D_800E1204[kind];                                          \
        if (kind == 4 && D_800F3428 != 0) palette += 4;                      \
        clut = func_80077AA4(0x60, palette);                                 \
        func_800CEE20(&position, 0, scale, scale, 0xE, clut, 1, fade, 0);    \
    }

/* Spark that rides the effect channel's wave slots: it projects itself
 * along the current and next slot, flashes briefly, then collapses into a
 * floor sprite and a coloured burst between the two projected points. */
int RoomEffect_WaveSparkParticle(int mode, RoomPhasedSpark *spark) {
    GteShortVector position;
    GteShortVector second;
    GteRotation rotationA = s_WaveOffsetA;
    GteRotation rotationB = s_WaveOffsetB;
    GteRotation rotationC = s_WaveOffsetC;
    RenderColor color;
    GteShortVector floorPos;
    GteRotation floorSpin;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int fade;
    int phase;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            if ((s16)spark->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0:
            phase = spark->phase;
            if (phase < 7) {
                func_800CE8F0(D_800F32D0->pool, phase + 1, &rotationA, &position);
                ROOMEFFECT_WAVE_SPARK_FLASH();
                ROOMEFFECT_WAVE_SPARK_BLOOM(1);
                func_800CE8F0(D_800F32D0->pool, spark->phase + 1, &rotationB, &position);
            } else {
                func_800CE8F0(D_800F32D0->pool, phase + 9, &rotationC, &position);
                ROOMEFFECT_WAVE_SPARK_FLASH();
                ROOMEFFECT_WAVE_SPARK_BLOOM(1);
                func_800CE8F0(D_800F32D0->pool, spark->phase + 13, &rotationC, &position);
            }
            ROOMEFFECT_WAVE_SPARK_FLASH();
            ROOMEFFECT_WAVE_SPARK_BLOOM(0);
            break;
        case 1:
            func_800CE8F0(D_800F32D0->pool, 11, &rotationC, &position);
            func_800CE8F0(D_800F32D0->pool, 15, &rotationC, &second);
            {
                u16 clut;
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x10;
                D_800F3368.parameter02 = 1;
                D_800F3368.extent_x = 0x10;
                D_800F3368.extent_y = 0x10;
                position.x = (position.x + second.x) / 2;
                position.y = (position.y + second.y) / 2;
                position.z = (position.z + second.z) / 2;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800CEE20(&position, 0, 0x2000, 0x2000,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0xA7,
                              clut, 1, 0x80, 0);
            }
            {
                u16 clut;
                int kind;
                int palette;
                floorSpin.x = 0x400;
                floorSpin.y = 0;
                floorSpin.z = 0;
                floorSpin.flags = 1;
                floorPos.x = position.x;
                floorPos.z = position.z;
                floorPos.y = D_800942EC.count;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x50, palette);
                func_800CEE20(&floorPos, &floorSpin, 0x1000, 0x1000,
                              (s16)D_800F3368.parameter02 * (s16)spark->timer + 0xA7,
                              clut, 3, 0x80, 0);
            }
            scale = rcos((s16)spark->timer << 7);
            func_800CF3AC(g_RoomWaveSparkColors, &color, (s16)spark->timer);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D0728(&position, 0x15E, 0x2BC, 0x18, 0, scale, scale, 0, &color,
                          0x80, 1);
            break;
        }
        break;
    }
    return 0;
}


/* Wave spark controller: plays the sound, then spawns one spark per frame
 * for eleven frames, each on the next joint; the last one collapses. */
int RoomEffect_WaveSparkController(int mode, s16 *counter)
{
    /* Retail reads these three stack halfwords without writing them here. */
    u16 seed[3];
    RoomPhasedSpark *particle;

    switch (mode) {
    case 0:
        *counter = 0;
        func_800D3F64(0x594, func_800D3FD8());
        if (D_800E2368->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)D_800F32D0->pool;
            if (nodes != 0) {
                RoomSparkNode *node = *nodes;
                if (node != 0) {
                    u8 *status = node->state;
                    if (*status == 1) *status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 24, 14, RoomEffect_WaveSparkParticle);
    case 1:
        if (*counter < 11) {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->x = seed[0];
                particle->y = seed[1];
                particle->z = seed[2];
                particle->timer = 0;
                particle->phase = *counter;
                if (*counter >= 10) particle->state = 1;
                else particle->state = 0;
            }
            ++*counter;
        }
        if (D_800E27EC < 10) break;
        return 2;
    case 2: {
        u16 tpage = D_800E2850[D_800E11EA];
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


/* Spark that is born between two wave slots, steers along two random
 * rotations, bounces on the floor and draws as a sprite with a coloured
 * trail. */
int RoomEffect_CometSparkParticle(int mode, RoomCometSpark *spark) {
    GteRotation rotationA = s_FloorRotation;
    GteRotation rotationB = s_HandOffset;
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
            LoadAverageShort12(spark, &spark->vx, 0xAAB, 0x555, &spark->steer);
            LoadAverageShort12(spark, &spark->vx, 0x556, 0xAAA, &spark->drift);
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
                RoomSparkBarragePacket *packet;
                if (((*(RoomSparkBarragePacket **)channel->pool)->code & 0x3F000000) ==
                    0x01000000) {
                    D_8009D254->actor->flags |= 0x4000;
                    packet = *(RoomSparkBarragePacket **)channel->pool;
                    packet->code = (packet->code & 0xC0FFFFFF) | 0x21000000;
                    packet = *(RoomSparkBarragePacket **)channel->pool;
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
                g_RoomCometSparkTrail.steer = spark->steer;
                g_RoomCometSparkTrail.drift = spark->drift;
                g_RoomCometSparkTrail.position = *(GteRotation *)spark;
                g_RoomCometSparkTrail.velocity = *(GteRotation *)&spark->vx;
                gte_ldrotmatrix(D_800BCFA4.value);
                gte_ldtransmatrix(D_800BCFA4.value);
                func_800CF3AC(g_RoomCometSparkColors, &color,
                              (s16)spark->timer * 2);
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                clut = func_80077AA4(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                       : palette + 2);
                func_800D3114(&g_RoomCometSparkTrail, 3, 0x12C, 0, 0x81, 0x7F,
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
            scale = rsin((s16)spark->timer << 6);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800CF3AC(g_RoomCometSparkColors, &color, (s16)spark->timer);
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


/* Controller for the comet sparks: anchors on the actor, marks the primary
 * channel's first node, then every fourth frame fires a burst of ten sparks
 * along a random direction until the countdown runs out. */
int RoomEffect_CometSparkController(int mode, RoomCometSparkAnchor *anchor) {
    GteShortVector direction;
    GteShortVector origin;
    RoomCometSpark *child;
    RoomSparkNode *node;
    int i;

    switch (mode) {
    case 0:
        anchor->count = 6;
        func_800CE870(D_800F32D0->pool, 0, anchor);
        if (D_800E2368->active) {
            if (D_800F32D0->pool) {
                node = *(RoomSparkNode **)D_800F32D0->pool;
                if (node) {
                    if (node->state[0] == 1) node->state[0] = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 0x24, 22,
                             RoomEffect_CometSparkParticle);
    case 1:
        if ((D_800E27EC & 3) == 0) {
            if (anchor->count <= 0) return 2;
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &direction);
            direction.z = 0;
            direction.x = 0;
            direction.y += (func_80071A54() & 0x1FF) - 0x100;
            func_800CFB7C(&direction, (s16)(func_80071A54() % 1400 + 1500), &origin);
            origin.x += anchor->x;
            origin.y += anchor->y;
            origin.z += anchor->z;
            origin.y = D_800942EC.count;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->vx = origin.x;
                child->vy = origin.y;
                child->vz = origin.z;
                child->state = 0;
                child->timer = 0;
            }
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = origin.x;
                child->y = origin.y;
                child->z = origin.z;
                child->state = 1;
                child->timer = 0;
            }
            for (i = 0; i < 8; i++) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    child->x = origin.x;
                    child->y = origin.y;
                    child->z = origin.z;
                    child->vx = func_80071A54() % 128 - 64;
                    child->vy = -(func_80071A54() % 128);
                    child->vz = func_80071A54() % 128 - 64;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            anchor->count--;
        }
        if (anchor->count > 0) break;
        return 2;
    case 2:
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}


/* Spark that is pushed upward each frame, bounces off the floor height and
 * draws as a fading sprite or a spinning coloured one. */
int RoomEffect_LiftedSparkParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = s_ZeroRotation;
    RenderColor color;
    int scale;
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
            fall = (u16)spark->vy + 5;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
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
            spark->vx = spark->vx * 255 / 256;
            spark->vz = spark->vz * 255 / 256;
            fall = (u16)spark->vy + 2;
            spark->vy = fall;
            if (spark->y >= D_800942EC.count) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 24) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            int intensity;
            intensity = rcos((s16)spark->timer << 6) / 32;
            scale = rcos((s16)spark->timer << 6) / 2;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x20, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale, 0xA0, clut, 1,
                          intensity, 0);
            break;
        }
        case 1: {
            int clut;
            int kind;
            int palette;
            scale = rsin(((s16)spark->timer << 10) / 24) * 2 + 0x2000;
            rcos(((s16)spark->timer * 1204) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            rotation.z = D_800E27EC << 5;
            func_800CF3AC(g_RoomLiftedSparkColors, &color,
                          (s16)spark->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, scale, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 4) + 0x20,
                          clut, 1, 0xA0, &color);
            break;
        }
        }
        break;
    }
    return 0;
}


/* Controller that sprays lifted sparks from the actor on odd frames of the
 * first minute, bursts sixteen more on frame 48, plays the sound on frame
 * two, and configures the sprite palette. */
int RoomEffect_LiftedSparkController(int mode) {
    GteShortVector target;
    RoomDampedSpark *child;
    char *pool;
    int handle;
    int i;

    switch (mode) {
    case 0:
        pool = D_800F33E0->pool;
        return func_800CE560(pool, 20, 32, RoomEffect_LiftedSparkParticle);
    case 1:
        pool = D_800F32D0->pool;
        func_800CE870(pool, 0, &target);
        if ((D_800E27EC & 1) && D_800E27EC < 60) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = target.x;
                child->y = target.y;
                child->z = target.z;
                child->vx = func_80071A54() % 80 - 40;
                child->vy = -(func_80071A54() % 60);
                child->vz = func_80071A54() % 80 - 40;
                child->state = 1;
                child->timer = 0;
            }
        }
        if (D_800E27EC == 48) {
            for (i = 0; i < 16; i++) {
                pool = D_800F33E0->pool;
                child = func_800CE610(pool);
                if (child) {
                    child->x = target.x;
                    child->y = target.y;
                    child->z = target.z;
                    child->vx = func_80071A54() % 192 - 96;
                    child->vy = -(func_80071A54() % 170);
                    child->vz = func_80071A54() % 192 - 96;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 2) {
            handle = func_800D3FD8();
            func_800D3F64(0x5B4, handle);
        }
        if (D_800E27EC < 2) break;
        return 2;
    case 2: {
        int index = D_800E11EA;
        int tpage = D_800E2850[index];
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
