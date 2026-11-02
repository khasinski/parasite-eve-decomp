/*
 * The spark ring and spark burst effects.
 *
 * Ten museum rooms link these five functions in this order, with the same
 * 0x54 bytes of read-only seeds and jump tables; this unit is that object,
 * compiled into each of them. The ring controller plays a sound, waits,
 * then sheds damped sparks around the actor while it draws a pulsing sprite
 * with two expanding rings. The burst spawner launches homing sparks from
 * the actor's joints; each sheds trail sparks and bursts into debris when it
 * reaches the actor, the floor or a wall. The colour ramp and the slot word
 * live in each room's own data.
 */

#include "pe1/field_texture_tables.h"
#include "pe1/gte.h"
#include "pe1/room_spark_ring_burst.h"

static const RoomSparkSeed s_SparkRingParticleSeed = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

/* Damped spark shed by the ring controller: mode 1 drifts it with damping
 * and a floor bounce for 24 frames, mode 2 draws it as a shrinking sprite
 * coloured from the room's ramp. */
int RoomEffect_SparkRingParticle(int mode, RoomSparkRingParticle *state) {
    RoomSparkSeed seed;
    RoomSparkSeed out;
    int scale;
    int texture;
    int frame;
    void *table;

    seed = s_SparkRingParticleSeed;
    switch (mode) {
    case 1:
    {
        unsigned short nextVy;
        int y;

        if (state->state != 0) {
            return 0;
        }
        state->timer++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vx = (state->vx * 0xFF) / 0x100;
        state->vz = (state->vz * 0xFF) / 0x100;
        {
            y = state->y;
            nextVy = (unsigned short)state->vy + 1;
            state->vy = nextVy;
            if (y >= g_RoomFloorY->y) {
                int bounce = (short)nextVy;
                state->vy = -bounce;
            }
        }
        if (state->timer < 24) {
            break;
        }
        return 1;
    }
    case 2:
        if (state->state != 0) {
            return 0;
        }
        scale = rsin((state->timer << 10) / 24) * 2 + 0x1000;
        rcos((state->timer * 1204) / 24);
        /* Retail loads the ramp address before the parameter stores and
         * stores the seed's angle before the colour lookup; the use and
         * the barrier keep both in place. */
        table = g_RoomSparkRingColorRamp;
        PE1_COMPILER_USE(table);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        frame = g_FieldFxCurrentAge;
        *(unsigned short *)&seed.bytes[4] = frame * 24;
        PE1_COMPILER_MEMORY_BARRIER();
        FieldFx_SampleColorTrack(table, &out, state->timer);
        texture = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
            texture += 4;
        }
        FieldFx_DrawShapeQuads((GteShortVector *)state, (GteRotation *)&seed,
                      scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * (state->timer / 3) + 0x20,
                      GetClut(0x10, texture),
                      1, 0xA0, (RenderColor *)&out);
        return 0;
    default:
        return 0;
    }

ret0:
    return 0;
ret1:
    return 1;
}

static const GteRotation s_SparkRingRotation = { 0, 0, 0, 0 };
static const RenderColor s_SparkRingShade = { 0x78, 0x78, 0x64, 0 };

typedef struct RoomSparkRingChild {
    s16 x, y, z, reserved06;
    s16 dx, dy, dz, reserved0E;
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
} RoomSparkRingChild;

/* Controller that plays a sound at the actor, waits twenty frames, then
 * emits randomized particles around the actor for thirty-two frames while
 * drawing a pulsing sprite with two expanding rings. */
int RoomEffect_SparkRingController(int mode, RoomSparkRingState *state) {
    GteRotation rotation = s_SparkRingRotation;
    GteShortVector target;
    RenderColor color;
    RenderColor shade = s_SparkRingShade;
    RoomSparkRingChild *child;
    RenderMatrixSlot *matrixSlot;
    char *pool;
    int handle;
    int kind;
    int palette;
    u16 clut;
    int width;
    int height;
    int intensity;
    int idx;
    int unit;
    u16 *tpageSlot;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, (s16 *)&state->target);
        handle = FieldFx_GetOwnerProgress();
        FieldFx_PlaySoundAtOwner(0x59E, handle);
        state->state = 0;
        state->timer = 0;
        if (FX_TASK_AS(RoomSparkEventState)->active) {
            RoomSparkNode **slot = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
        pool = ANIM_POOL_AS(RoomSparkChannel)->pool;
        return FieldFx_InitCallbackList(pool, 20, 24,
                             (FieldFxCallbackListCallback)RoomEffect_SparkRingParticle);
    case 1:
        switch (state->state) {
        case 0:
            state->timer++;
            if (state->timer < 20) break;
            state->state = 1;
            state->timer = 0;
            break;
        case 1:
            state->timer++;
            pool = ANIM_POOL_AS(RoomSparkChannel)->pool;
            child = FieldFx_AcquireCallbackSlot(pool);
            if (child) {
                child->x = state->target.x;
                child->y = state->target.y;
                child->z = state->target.z;
                child->x += (rand() & 0xFF) - 0x80;
                child->z += (rand() & 0xFF) - 0x80;
                child->y += (rand() & 0x1FF) - 0x100;
                child->dx = rand() % 32 - 16;
                child->dy = rand() % 32 - 16;
                child->dz = rand() % 32 - 16;
                child->state = 0;
                child->timer = 0;
            }
            if (FieldFx_IsPlayerWithinRadius(&state->target, 180)) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        ((RoomSparkBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x21000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
            }
            if (state->timer < 32) break;
            state->state = 2;
            break;
        case 2:
            return 2;
        }
        break;
    case 2:
        {
            int index;
            int tpage;
            unit = 0x20;
            index = TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot4);
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.parameter00 = unit;
            g_FieldFxSpriteParams.extent_x = unit;
            g_FieldFxSpriteParams.extent_y = unit;
            g_FieldFxSpriteParams.extent_x = 0x100;
            g_FieldFxSpriteParams.extent_y = unit;
            tpage = g_FieldFxTpageTable[index];
            g_FieldFxSpriteParams.palette = 4;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = unit;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        switch (state->state) {
        case 0:
            pool = ANIM_OWNER_AS(RoomSparkChannel)->pool;
            FieldFx_TransformMatrixPoint(pool, 16, &rotation, &target);
            width = rsin((state->timer << 10) / 20);
            height = rcos((state->timer << 10) / 20) / 2;
            FieldFx_SampleColorTrack(g_RoomSparkRingColorRamp, &color, state->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads(&target, 0, width, height, 0, clut, 1, 0x80, &color);
            matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            intensity = rcos((state->timer << 10) / 20) / 64;
            FieldFx_DrawStarFan(&target, 0x44C, 0x44C, 16, 0, 0x1000, 0x1000, &shade,
                          0, intensity, 1);
            FieldFx_DrawStarFan(&target, 0x1F4, 0x1F4, 8, 0, 0x1000, 0x1000, &shade,
                          0, intensity, 1);
            break;
        case 1:
            break;
        case 2:
            break;
        }
        /* Retail addresses the closing stores and the unit read through a
         * register holding the tpage slot; the empty constraint keeps the
         * compiler from folding it back into absolute addresses. */
        tpageSlot = &g_FieldFxSpriteParams.tpage;
        asm("" : "=r"(tpageSlot) : "0"(tpageSlot));
        idx = TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot4);
        unit = tpageSlot[-4];
        palette = g_FieldFxTpageTable[idx];
        g_FieldFxSpriteParams.palette = 4;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        tpageSlot[1] = 0;
        tpageSlot[2] = 0x10;
        tpageSlot[0] = palette;
        break;
    }
    return 0;
}

static const RenderColor s_BurstSparkGlow = { 0x0A, 0x78, 0xBE, 0 };
static const RenderColor s_BurstSparkShade = { 0x00, 0x1E, 0x78, 0 };

/* Particle callback of the burst spawner. State 0 flies along its heading,
 * drops a trail spark every other frame and bursts into six debris sparks
 * when it reaches the actor, the floor or a wall; trail (1) and debris (3)
 * sparks drift with damping and bounce off the floor; states 2 and 4 are
 * timed fades. Mode 2 draws each state as scaled sprites. */
int RoomEffect_BurstSparkParticle(int mode, RoomBurstSpark *spark,
                                  s16 *distance) {
    GteShortVector target;
    RenderColor glow = s_BurstSparkGlow;
    RenderColor shade = s_BurstSparkShade;
    GteShortVector ground;
    GteShortVector flat;
    RoomBurstSpark *child;
    char *pool;
    int i;
    int kind;
    int palette;
    u16 clut;
    int opacity;
    int scale;
    int fall;
    int bounce;
    int one;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            FieldFx_AnglesToVector((GteShortVector *)&spark->vx, *distance, &target);
            spark->x += target.x;
            spark->y += target.y;
            spark->z += target.z;
            if (!(spark->timer & 1)) {
                pool = ANIM_POOL_AS(RoomSparkChannel)->pool;
                child = FieldFx_AcquireCallbackSlot(pool);
                if (child) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = rand() % 32 - 16;
                    child->vy = rand() % 32 - 16;
                    child->vz = rand() % 32 - 16;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (FieldFx_IsPlayerWithinRadius(spark, 50) && g_RoomFloorY->y - 0x202 < spark->y) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    u32 *entry;
                    if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                        ((RoomSparkBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                        entry = *(u32 **)channel->pool;
                        *entry = (*entry & 0xC0FFFFFF) | 0x19000000;
                        entry = *(u32 **)channel->pool;
                        *entry |= 0x80000000;
                    }
                }
                spark->state = 2;
                spark->timer = 0;
                for (i = 0; i < 6; i++) {
                    pool = ANIM_POOL_AS(RoomSparkChannel)->pool;
                    child = FieldFx_AcquireCallbackSlot(pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->vx = rand() % 40 - 20;
                        child->vy = rand() % 40 - 20;
                        child->vz = rand() % 40 - 20;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (spark->y >= g_RoomFloorY->y) {
                spark->state = 2;
                spark->timer = 0;
                for (i = 0; i < 6; i++) {
                    pool = ANIM_POOL_AS(RoomSparkChannel)->pool;
                    child = FieldFx_AcquireCallbackSlot(pool);
                    if (child) {
                        child->x = spark->x;
                        child->y = spark->y;
                        child->z = spark->z;
                        child->vx = rand() % 40 - 20;
                        child->vy = rand() % 40 - 20;
                        child->vz = rand() % 40 - 20;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
            }
            if (Geo_PointInPoly(spark->x << 16, spark->z << 16, g_FloorPolygonVertices,
                              g_FloorPolygonVertexCount) == 0) {
                spark->state = 2;
                spark->timer = 0;
            }
            break;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 6) break;
            return 1;
        case 3:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 12) break;
            return 1;
        case 2:
        case 4:
            spark->timer++;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        switch (spark->state) {
        case 0:
            opacity = 0x80 - ((spark->timer & 1) << 6);
            target.x = 0;
            target.y = 0;
            target.z = (s16)spark->timer * 128;
            target.pad = 0;
            /* Every draw fills the parameter block and then reads the
             * palette back through it: cse keeps the block address in s0,
             * so parameter00 is stored through it and the later reads are
             * addressed off it, while the other stores fold to absolute
             * addresses as in retail. */
            {
                int kind;
                int palette;

                g_FieldFxSpriteParams.parameter00 = 0x10;
                g_FieldFxSpriteParams.parameter02 = 1;
                g_FieldFxSpriteParams.extent_x = 0x10;
                g_FieldFxSpriteParams.extent_y = 0x10;
                g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 0;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x20, palette);
            }
            FieldFx_DrawShapeQuads((GteShortVector *)spark, (GteRotation *)&target,
                          0x1000, 0x1000, 0xD8, clut, 1, opacity, &glow);
            {
                u16 kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x20, palette);
            }
            FieldFx_DrawShapeQuads((GteShortVector *)spark, (GteRotation *)&target,
                          0x2000, 0x2000,
                          (s16)g_FieldFxSpriteParams.parameter02 + 0xD8, clut, 1,
                          opacity / 2, &shade);
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.pad = 1;
            ground.x = spark->x;
            ground.z = spark->z;
            ground.y = g_RoomFloorY->y;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&ground, (GteRotation *)&flat, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 + 0xD8, clut, 1,
                          opacity, &shade);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;

            opacity = rsin((s16)spark->timer << 7) / 64 + 0x40;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (s16)spark->timer + 0xE0,
                          clut, 1, opacity, &shade);
            break;
        }
        case 2:
            opacity = rcos((s16)spark->timer << 6) / 32;
            if (spark->timer & 1) opacity = opacity * 2 / 3;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot4)];
            g_FieldFxSpriteParams.palette = 4;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            one = 1;
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0x1800, 0x1800,
                          (s16)g_FieldFxSpriteParams.parameter02 *
                              ((s16)spark->timer / 2) + 0xC0,
                          clut, one, opacity, &glow);
            scale = rsin((s16)spark->timer << 6) + 0x800;
            FieldFx_DrawStarFan((GteShortVector *)spark, 0x12C, 0x12C, 5, 0,
                          scale, scale, &shade, 0, opacity / 2, one);
            return 0;
        case 3: {
            u16 clut;
            int kind;
            int palette;
            int step;

            opacity = rcos(((s16)spark->timer << 10) / 12) / 32;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            step = (s16)spark->timer * 2 / 3;
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * step + 0xC8, clut, 1,
                          opacity, &shade);
            break;
        }
        case 4: {
            u16 clut;
            int kind;
            int palette;

            scale = rsin((s16)spark->timer << 6) + 0x1000;
            opacity = rcos((s16)spark->timer << 6) / 32;
            spark->vw = 1;
            spark->vz = (s16)spark->timer * 24;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x90, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark,
                          (GteRotation *)&spark->vx, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xFD, clut, 1,
                          opacity, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

static const RoomSparkSeed s_BurstSparkLaunchOffset = {
    { 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00 }
};

/* Burst spawner: plays a sound, then every third frame launches a homing
 * spark and a fading spark from the actor's joints, eight times. */
int RoomEffect_BurstSparkSpawner(int mode, int *count) {
    RoomSparkSeed offset;
    RoomSparkLaunch launch;
    RoomBurstSpark *spark;
    RoomSparkChannel *channel;

    offset = s_BurstSparkLaunchOffset;
    switch (mode) {
    case 0:

        if (FX_TASK_AS(RoomSparkEventState)->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (nodes != 0 && *nodes != 0) {
                unsigned char *state = (*nodes)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        FieldFx_PlaySoundAtOwner(0x59E, FieldFx_GetOwnerProgress());
        channel = ANIM_POOL_AS(RoomSparkChannel);
        *count = 0;
        return FieldFx_InitCallbackList(channel->pool, 0x14, 0x37,
                             (FieldFxCallbackListCallback)RoomEffect_BurstSparkParticle);

        case 1:
    FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 0x11, &offset, &launch);
    FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 0,
                  (GteShortVector *)&launch.hx);
    launch.hx -= 0x200;
    if (g_FieldFxCurrentAge % 3 == mode && *count < 8) {
        spark = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
        if (spark != 0) {
            spark->x = launch.position.x;
            spark->y = launch.position.y;
            spark->z = launch.position.z;
            spark->vx = launch.hx;
            spark->vy = launch.hy;
            spark->vz = launch.hz;
            spark->state = 0;
            spark->timer = 0;
        }
        spark = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
        if (spark != 0) {
            spark->x = launch.position.x;
            spark->y = launch.position.y;
            spark->z = launch.position.z;
            spark->vx = launch.hx;
            spark->vy = launch.hy;
            spark->vz = launch.hz;
            spark->state = 4;
            spark->timer = 0;
        }
        (*count)++;
    }
        if (g_FieldFxCurrentAge >= 8) return 2;
    break;
    case 2:

        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
    
    }
    return 0;
}

/* Stores the second argument in the room's slot word and returns it. */
int *RoomEffect_SetSparkSlot(int unused, int value) {
    int *slot = &g_RoomSparkSlot;
    *slot = value;
    return slot;
}
