#include "common.h"
#include "pe1/field_texture_tables.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/room_m256.h"
#include "pe1/room_m256_drop.h"
#include "pe1/room_m256_effects.h"
#include "pe1/room_m256_swing.h"
#include "pe1/room_m256_trail.h"
#include "pe1/room_model_pulse.h"
#include "pe1/room_orbiting_pair.h"

#include "m256_externs.h"

/* Cosine halves of the packed sine table words (pe1/gte_sine_table.h). */

void RoomM256_SwingStep(RoomM256Swing *state);

void RoomM256_SwingAwaitMode(RoomM256Swing *obj) {
    FieldActor *slot = obj->actor;

    if (slot->mode == 0xB) {
        obj->cycle.callback = RoomM256_SwingStep;
    }
}

void RoomM256_SwingStep(RoomM256Swing *state) {
    FieldActor *actor = state->actor;
    RoomM256SwingCycle *base = &state->cycle;
    int first;
    int phase;
    int value;
    int timer;
    int baseValue;

    if (state->cycle.started == 0) {
        state->cycle.started = 1;
        state->cycle.tick = 0;
        first = base->angles[state->cycle.segment];
        state->cycle.startAngle = first;
        state->cycle.sweep = base->angles[state->cycle.segment + 1] - first;
        state->cycle.length = base->durations[state->cycle.segment];
    }

    timer = (u16)state->cycle.tick + 1;
    phase = ((timer << 16) >> 5) / state->cycle.length;
    state->cycle.tick = timer;
    value = (-g_RcossinTblCos[(phase & 0xFFF) * 2] + 0x1000) *
            state->cycle.sweep;
    if (value < 0) {
        value += 0x1FFF;
    }

    baseValue = (u16)state->cycle.startAngle;
    actor->render_object.rotation_overrides[0].matrix_index = 3;
    actor->render_object.rotation_overrides[0].flags = 2;
    actor->render_object.rotation_overrides[0].y = baseValue + (value >> 13);

    if (state->cycle.tick >= state->cycle.length) {
        state->cycle.segment = (u16)state->cycle.segment + 1;
        if (state->cycle.segment >= 3) {
            RoomM256_SwingRelease(state);
        } else {
            state->cycle.started = 0;
        }
    }

    if (actor->anim.parts.integer >= actor->action) {
        actor->anim.fixed = 0;
    }
}

int RoomM256_SwingRelease(RoomM256Swing *obj) {
    int *state;

    obj->actor->render_object.rotation_overrides[0].matrix_index = 0;
    obj->actor->render_object.rotation_overrides[0].flags = 0;
    state = obj->cycle.signal;
    obj->status = 4;
    if (state != 0) {
        *state = 0;
    }

    return 0;
}

int RoomM256_SwingNop6(void) {
    return 0;
}

/* Flashes the scene when the drop touches the player. */
#define ROOM_M256_DROP_FLASH(radius)                                          \
    if (FieldFx_IsPlayerWithinRadius(drop, radius) && drop->hazard && FX_TASK_AS(RoomM256DropEvent)->active) {  \
        RoomM256DropChannel *channel = ANIM_OWNER_AS(RoomM256DropChannel);                            \
        if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {        \
            ((RoomM256DropEntity *)D_8009D254)->actor->flags |= 0x4000;                               \
            channel->pool->node->flags =                                      \
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x19000000;       \
            channel->pool->node->flags |= 0x80000000;                         \
        }                                                                     \
    }

/* Scatter drop: falls and jitters, sheds three kinds of splash children,
 * flashes the scene on contact with the player and expires once it leaves
 * the walkable area; the splash children slow down and bounce. */
int RoomM256_ScatterDropCallback(int mode, RoomM256Drop *drop) {
    GteRotation rotation = g_RoomM256SpriteTilt;
    RoomM256DropColor color = g_RoomM256DropColor;
    RoomM256Drop *child;
    RenderMatrixSlot *slot;
    int width;
    int height;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (drop->state) {
        case 0:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0x1F) - 0x10;
            drop->y += (rand() & 0x1F) - 0x10;
            drop->z += (rand() & 0x1F) - 0x10;
            if (drop->y >= g_RoomFloorY->y - 0x10 || drop->timer >= 0x18) {
                drop->state = 1;
                drop->y = g_RoomFloorY->y - 0x10;
                drop->vy = 0;
            }
            if (drop->timer & 1) {
                if ((rand() & 7) == 0) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = drop->vx / 2 + (rand() & 0x1F) - 0x10;
                        child->vy = drop->vy / 2 + (rand() & 0x1F) - 0x10;
                        child->vz = drop->vz / 2 + (rand() & 0x1F) - 0x10;
                        child->state = 2;
                        child->timer = 0;
                    }
                }
                if ((rand() & 7) == 1) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (rand() & 0x1F) - 0x10;
                        child->vy = (rand() & 0x1F) - 0x10;
                        child->vz = (rand() & 0x1F) - 0x10;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
                if ((rand() & 0xF) == 2) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (rand() & 0x1F) - 0x10;
                        child->vy = (rand() & 0x1F) - 0x18;
                        child->vz = (rand() & 0x1F) - 0x10;
                        child->state = 4;
                        child->timer = 0;
                    }
                }
            }
            ROOM_M256_DROP_FLASH(0x19A);
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 1:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0x1F) - 0x10;
            drop->z += (rand() & 0x1F) - 0x10;
            ROOM_M256_DROP_FLASH(0x26C);
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 2:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 127 / 128;
            drop->vz = drop->vz * 127 / 128;
            fall = (u16)drop->vy - 2;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 3:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 511 / 512;
            drop->vz = drop->vz * 511 / 512;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 4:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 31 / 32;
            drop->vz = drop->vz * 31 / 32;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        }
        break;
    case 2:
        slot = &D_800BCFA4;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        switch (drop->state) {
        case 0:
            width = (rsin(drop->timer << 5) + 0x1200) * 2;
            height = (rsin(drop->timer << 5) + 0xE00) * 2;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, width, height,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 4),
                              GetClut(0, palette), 1, 0x72, 0);
            }
            if (rand() & 3) return 0;
            FieldFx_DrawStarFan((GteShortVector *)drop, 0xFA, 0xFA, 5, 0, width, width, (struct RenderColor *)&color, 0,
                          rcos(drop->timer << 5) / 32, 3);
            return 0;
        case 1:
            width = (rsin(drop->timer << 5) + 0x1200) * 2;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, &rotation, width, width,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 4) + 0x20,
                              GetClut(0x10, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            width = rsin(drop->timer << 6) / 2 + 0x1200;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, width, width,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 2),
                              GetClut(0x10, palette), 1, 0x60, 0);
            }
            return 0;
        case 3:
            width = rsin(drop->timer << 6) / 2 + 0x1000;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, width, width,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 2) + 0x40,
                              GetClut(0x20, palette), 2, 0x50, 0);
            }
            break;
        case 4:
            width = rcos(drop->timer << 6) / 32 + 0x40;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            FieldFx_DrawHaloedPoint(drop, &color, width, 1);
            break;
        }
        break;
    }
    return 0;
}

#undef ROOM_M256_DROP_FLASH

#undef ROOM_M256_DROP_FLASH

/* The stack templates the effects below copy: read-only data of this unit,
 * which GCC emits before the unit's jump tables, as retail has them. */
const RoomOrbitPairTemplate8 g_RoomM256ScatterDropAnchor = {{0, 0xFFE2}};
const RoomM256Template g_RoomM256SwirlSparkAnchor = {{0, 0xFFBA}};
const RenderColor g_RoomM256TrailColor = {0xC8, 0xBE, 0x18, 0};
const RenderColor g_RoomM256TrailDarkColor = {0x0A, 0x80, 0xF0, 0};
const GteRotation g_RoomM256TrailAnchor = {0, 0, -10, 0};

int RoomM256_ScatterDropEmitter(int mode, u16 *counter) {
    RoomOrbitPairTemplate8 template = g_RoomM256ScatterDropAnchor;
    s16 position[4];
    s16 target[4];
    s16 angles[4];
    char *pool;
    RoomOrbitSpark *child;
    int distance;
    int spread;
    switch (mode) {
    case 0:
    {
        RoomOrbitPairEventState *event = FX_TASK_AS(RoomOrbitPairEventState);
        *counter = 0;
        if (event->active) {
            RoomOrbitPairNode **slot = (RoomOrbitPairNode **)ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
    }
    pool = ANIM_POOL_AS(RoomOrbitPairChannel)->pool;
    return FieldFx_InitCallbackList(pool, 20, 50,
                         (FieldFxCallbackListCallback)RoomM256_ScatterDropCallback);
    case 1:
    pool = ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
    FieldFx_TransformMatrixPoint(pool, 4, &template, position);
    pool = ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
    FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)pool, 4, (GteShortVector *)target);
    target[0] -= 0xDC;
    if (g_FieldFxCurrentAge < 110 && (g_FieldFxCurrentAge & 1)) {
        pool = ANIM_POOL_AS(RoomOrbitPairChannel)->pool;
        child = FieldFx_AcquireCallbackSlot(pool);
        if (child) {
            distance = (rand() & 7) + 0x37;
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            angles[0] = target[0];
            angles[1] = target[1];
            angles[2] = target[2];
            spread = rand() & 0x1F;
            angles[1] += spread - 0x10;
            FieldFx_AnglesToVector((GteShortVector *)angles, distance, (GteShortVector *)&child->vx);
            child->parity = *counter & 1;
            child->state = 0;
            child->timer = 0;
        }
        (*counter)++;
    }
    if (g_FieldFxCurrentAge < 2) break;
    return 2;
    case 2:
    {
        int idx = g_FieldFxTpageSlot3;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        palette = g_FieldFxTpageTable[idx];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = palette;
    }
        break;
    }
    return 0;
}

/* Spark that jitters sideways (state 0) or swirls on a spinning circle
 * (state 1) while it falls with damped velocity and bounces off the
 * floor height; draws as a fixed or a pulsing sprite. */
int RoomM256_SwirlSparkCallback(int mode, RoomM256SwirlSpark *spark) {
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
            spark->x += (rand() & 1) * 2 - 1;
            spark->z += (rand() & 1) * 2 - 1;
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
            if (spark->y >= g_RoomFloorY->y) {
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
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0x1800, 0x1800,
                          (s16)g_FieldFxSpriteParams.parameter02 * (s16)spark->timer,
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
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)spark->timer / 2) + 0x40,
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
int RoomM256_SwirlSparkEmitter(int mode, s16 *counter) {
    RoomM256Template template = g_RoomM256SwirlSparkAnchor;
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
        soundSlot = g_LoadedSceneAssetBlock;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(*soundSlot, 0x5A7, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5BF, 0x80, 0x80, volume);
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x14, 0x20,
                             (FieldFxCallbackListCallback)RoomM256_SwirlSparkCallback);
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &template, &position);
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &base);
        if (g_FieldFxCurrentAge < 0x6F) {
            child = (RoomM256SwirlSpark *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter) {
                case 0:
                    distance = (rand() & 7) + 14;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0x7F) - 0x40;
                    break;
                case 1:
                    distance = (rand() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0x7F) + 0x3C0;
                    break;
                case 2:
                    distance = (rand() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (rand() & 0x7F) + 0x3C0;
                    break;
                }
                FieldFx_AnglesToVector(&angles, distance, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
            child = (RoomM256SwirlSpark *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter & 1) {
                case 0:
                    distance = (rand() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0xFF) + 0x380;
                    break;
                case 1:
                    distance = (rand() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (rand() & 0xFF) + 0x380;
                    break;
                }
                FieldFx_AnglesToVector(&angles, distance, (GteShortVector *)&child->vx);
                child->small = rand() & 1;
                child->state = 1;
                child->timer = 0;
            }
            *counter = (s16)(*counter + 1) % 3;
        }
        if (g_FieldFxCurrentAge < 2) {
            break;
        }
        return 2;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        break;
    }
    return 0;
}

s32 RoomM256_TrailCallback(int mode, void *effect) {
    RoomM256TrailEffect *state = effect;
    RenderColor color = g_RoomM256TrailColor;
    RenderColor darkColor = g_RoomM256TrailDarkColor;
    GteRotation initialRotation = g_RoomM256TrailAnchor;
    GteRotation spriteRotation = g_RoomM256SpriteTilt;
    RoomM256TrailEffect *particle;
    GteShortVector delta;
    GteShortVector angles;
    GteMatrix transform;
    GteVector scale;
    /* Unused space; the retail frame is 0xF0 bytes. */
    s16 unusedVector[12];
    s16 impactFrame;
    s16 historyIndexX;
    s16 historyIndexY;
    s16 historyIndexZ;
    s32 nextModelFrame;
    s32 nextSpriteFrame;
    s16 renderKind;
    s16 ending;
    s16 turnAngle;
    s16 speedBeforeStep;
    s16 speedAfterStep;
    s16 nextEnding;
    s16 updateKind;
    FieldActorState *trailActorState;
    FieldActorState *trailActorFlags;
    FieldActorState *impactActorState;
    FieldActorState *impactActorFlags;
    s32 impactScale;
    s32 randomVelocityX;
    s32 randomVelocityY;
    s32 modelPage;
    s32 pulseCosine;
    s32 impactCosine;
    s32 randomVelocityZ;
    s32 turnBlend;
    s32 squaredSpeed;
    s32 alive;
    RenderEffectParameters *params;
    RoomM256TrailPalettes *palettes;
    u16 *paletteIndex;
    s16 *floorHeight;
    u16 tpage;
    int clut;
    s16 impactY;
    int historyCount;
    void *historyPosition;
    GteShortVector *historySample;
    s16 *spriteFrame;
    register int pulseExtent asm("$3");
    int spriteExtent;
    unsigned spritePalette;
    int spriteClutY;
    int translationZ;
    int renderPalette;
    s32 random;
    s32 renderScale;
    s32 fade;
    register int firstFade asm("$21");
    u16 initialPitch;
    u16 initialYaw;
    int trailClutY;
    int pulseClutY;
    int impactClutY;
    int ringClutY;
    int modelClutY;
    u32 tripledScale;
    u32 pulseSine;
    void *motionAngles;
    RoomM256TrailHistory *history;
    FieldActorState *trailPlayerState;
    FieldActorState *impactPlayerState;

    switch (mode) {
    case 1:
        updateKind = state->kind;
        switch (updateKind) {
        case 0:
            state->frame = 0;
            state->kind = 1U;
            state->ending = 0;
            FieldFx_TransformMatrixPoint(g_FieldFxOwner->actor, 2, &initialRotation, state);
            FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)g_FieldFxOwner->actor, 0,
                          (GteShortVector *)&angles.x);
            historyCount = 16;
            historyPosition = state;
            initialPitch = angles.x + 0x200;
            angles.x = initialPitch;
            {
                int y = (u16)angles.y - 0x200;
                int shiftedIndex = state->index << 9;
                initialYaw = y + shiftedIndex;
            }
            angles.y = initialYaw;
            state->motionX = initialPitch;
            state->motionY = angles.y;
            state->motionZ = angles.z;
            state->motionY = (u16) (state->motionY + 0x800);
            FieldFx_UpdatePointHistory(&g_RoomM256TrailHistory[state->index], historyCount, historyPosition, 1);
            state->speed = (s16) ((rand() & 7) + 8);
            state->angle = 0;
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);

            if (particle != 0) {
                particle->x = (u16) state->x;
                particle->y = (u16) state->y;
                {
                    int z = (u16)state->z;
                    particle->kind = 4;
                    particle->frame = 0;
                    particle->z = z;
                }

            }
            return 0;
        case 1:
            state->frame = (s16) ((u16) state->frame + 1);
            history = &g_RoomM256TrailHistory[state->index];
            FieldFx_GetActorPosition((char *)g_PlayerEntity, 1, &delta.x);
            FieldFx_CalculateLookAngles(state, &delta.x, &angles.x);
            ending = state->ending;
            if (ending == 0) {
                if (state->speed >= 0x1B) {
                    turnAngle = state->angle;
                    if (turnAngle < 0x800) {
                        state->angle = (s16) (turnAngle + 0x50);
                    }
                    if (state->angle >= 0x801) {
                        state->angle = 0x800;
                    }
                }
                turnBlend = rsin(state->angle) * g_RoomM256TrailTurnScale;
                if (turnBlend < 0) {
                    turnBlend += 0xFFF;
                }
                motionAngles = &state->motionX;
                FieldFx_EaseAngles(&angles.x, motionAngles, (turnBlend >> 0xC) & 0xFFFF);
                speedBeforeStep = state->speed;
                if (speedBeforeStep < 0x32) {
                    state->speed = (s16) (speedBeforeStep + 6);
                }
                speedAfterStep = state->speed;
                squaredSpeed = speedAfterStep * speedAfterStep;
                if (squaredSpeed < 0) {
                    squaredSpeed += 0xF;
                }
                FieldFx_AnglesToVector(motionAngles, (squaredSpeed << 0xC) >> 0x10, (GteShortVector *)&delta.x);
                state->x = (s16) ((u16) state->x + delta.x);
                state->y = (s16) ((u16) state->y + delta.y);
                state->z = (s16) ((u16) state->z + delta.z);
            } else {
                nextEnding = ending + 1;
                state->ending = nextEnding;
                if (nextEnding >= 0x10) return 1;
            }
            {
                FieldFx_UpdatePointHistory(history, 0x10, state, 0);
                if ((FieldFx_IsPlayerWithinRadius(state, 0x64) != 0) && (state->y >= (g_RoomFloorY->y - 0x202)) && (g_FieldFxTaskContext->flags != 0) && ((g_FieldFxOwner->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                    trailPlayerState = g_PlayerEntity->state;
                    trailPlayerState->flags |= 0x4000;
                    trailActorState = g_FieldFxOwner->actor->state;
                    trailActorState->core_flags = (trailActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                    trailActorFlags = g_FieldFxOwner->actor->state;
                    trailActorFlags->core_flags |= 0x80000000;
                }
                if (state->ending == 0) {
                    if (g_FieldFxCurrentAge == (((g_FieldFxCurrentAge / 3)) * 3)) {
                        particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            particle->motionX = (u16) state->motionX;
                            particle->motionY = (u16) state->motionY;
                            {
                                int x = (u16)particle->motionX;
                                int z = (u16)state->motionZ;
                                particle->frame = 0;
                                particle->motionX = x + 0x800;
                                particle->kind = 3;
                                particle->motionZ = z;
                            }
                        }
                    }
                    particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                    if (particle != 0) {
                        random = rand();
                        historyIndexX = state->index;
                        particle->x = (u16) *(s16 *)(g_RoomM256TrailPoint4X + (((historyIndexX * 0x11) + (random & 3)) * 8));
                        random = rand();
                        historyIndexY = state->index;
                        particle->y = (u16) *(s16 *)(g_RoomM256TrailPoint4Y + (((historyIndexY * 0x11) + (random & 3)) * 8));
                        random = rand();
                        historyIndexZ = state->index;
                        particle->z = (u16) *(s16 *)(g_RoomM256TrailPoint4Z + (((historyIndexZ * 0x11) + (random & 3)) * 8));
                        particle->x = (u16) (particle->x - 0x40 + (rand() & 0x7F));
                        particle->y = (u16) (particle->y - 0x40 + (rand() & 0x7F));
                        particle->z = (u16) (particle->z - 0x40 + (rand() & 0x7F));
                        randomVelocityX = rand();
                        particle->motionX = (s16) (((randomVelocityX % 60)) - 0x1E);
                        randomVelocityY = rand();
                        particle->motionY = (s16) (((randomVelocityY % 60)) - 0x1E);
                        randomVelocityZ = rand();
                        particle->kind = 2;
                        particle->frame = 0;
                        particle->motionZ = (s16) (((randomVelocityZ % 60)) - 30);
                    }
                }
                floorHeight = &g_RoomFloorY->y;
                if ((state->y >= *floorHeight) || (((g_FieldFxCurrentAge < 0x51) == 0))) {

                    if (state->ending == 0) {
                        particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            impactY = *floorHeight;
                            particle->kind = 5;
                            particle->frame = 0;
                            particle->y = (u16) impactY;
                        }
                        state->ending = 1;
                        return 0;
                    }
                }
            }

            return 0;
        case 2:
            {
                register unsigned frame asm("$3") = *(volatile u16 *)&state->frame;
                register int vx asm("$6") = state->motionX;
                register unsigned coordinate asm("$2");
                register unsigned vxBits asm("$5");
                int dampX;
                unsigned z;
                unsigned vy;
                unsigned vz;
                int dampZ;
                coordinate = *(volatile u16 *)&state->x;
                vxBits = *(volatile u16 *)&state->motionX;
                asm volatile("" : :);
                dampX = vx * 63;
                state->x = coordinate + vxBits;
                coordinate = *(volatile u16 *)&state->y;
                vy = *(volatile u16 *)&state->motionY;
                state->frame = frame + 1;
                z = (u16)state->z;
                vz = (u16)state->motionZ;
                state->y = coordinate + vy;
                state->z = z + vz;
                state->motionX = dampX / 64;
                dampZ = state->motionZ * 63;
                if (dampZ < 0) dampZ += 63;
                vy = (u16)state->motionY;
                state->motionZ = dampZ >> 6;
                { int positionY;
                positionY = state->y;
                state->motionY = vy;

                if (positionY >= g_RoomFloorY->y) {
                    int signedY = (s16)vy;
                    state->motionY = -signedY;
                }
            }
                }
            alive = state->frame < 0x10;
            if (alive == 0) {
                return 1;
            }
            break;
        case 3:
            nextModelFrame = (u16) state->frame + 1;
            state->frame = nextModelFrame;
            asm("" : "=r"(nextModelFrame) : "0"(nextModelFrame));
            alive = (s16)nextModelFrame < 0xA;
            if (alive == 0) {
                return 1;
            }
            break;
        case 4:
            nextSpriteFrame = (u16) state->frame + 1;
            state->frame = nextSpriteFrame;
            asm("" : "=r"(nextSpriteFrame) : "0"(nextSpriteFrame));
            alive = (s16)nextSpriteFrame < 0x10;
            if (alive == 0) {
                return 1;
            }
            break;
        case 5:
            state->frame = (s16) ((u16) state->frame + 1);
            if ((FieldFx_IsPlayerWithinRadius(state, 0xB4) != 0) && (state->frame < 0x10) && (g_FieldFxTaskContext->flags != 0) && ((g_FieldFxOwner->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                impactPlayerState = g_PlayerEntity->state;
                impactPlayerState->flags |= 0x4000;
                impactActorState = g_FieldFxOwner->actor->state;
                impactActorState->core_flags = (impactActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                impactActorFlags = g_FieldFxOwner->actor->state;
                impactActorFlags->core_flags |= 0x80000000;
            }
            alive = state->frame < 0x18;
checkLifetime:

            if (alive == 0) {
                return 1;
            }

            break;
        }
        break;
    case 2:
        renderKind = state->kind - 1;
        switch (renderKind) {
        case 0:
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            trailClutY = g_FieldFxPaletteTable[D_800F336C];
            if ((D_800F336C == 4) && (g_FieldFxAltPaletteFlag != 0)) {
                trailClutY += 4;
            }
            FieldFx_DrawTexturedRibbon(g_RoomM256TrailHistory[state->index].point, 0x10, 0xF0, 0, 0x60, 0xFF, 0xF, g_FieldFxTpageTable[g_FieldFxTpageSlot3], GetClut(0x30, trailClutY) & 0xFFFF, 0x80, (RenderColor *)&color.r, (RenderColor *)&darkColor.r, 1);
            fade = ((g_FieldFxCurrentAge & 1) << 5) + 0x60;
            FieldFx_DrawStarFan((GteShortVector *)state, 0x190, 0x190, 8, 0, 0x1000, 0x1000, (RenderColor *)&color.r, 0, fade, 1);
            historySample = g_RoomM256TrailHistory[0].point + (rand() & 7);
            FieldFx_DrawStarFan((void *)((unsigned)(state->index * 0x88) + (unsigned)historySample), 0x1F4, 0x1F4, 6, 0, 0x1000, 0x1000, (RenderColor *)&color.r, 0, (unsigned)fade >> 1, 1);
            return 0;
        case 1:
            firstFade = rcos(state->frame << 6) / 32;
            renderScale = rcos(state->frame << 6);
            asm volatile("" : : "m"(g_FieldFxSpriteParams));
            params = &g_FieldFxSpriteParams;
            spriteExtent = 0x10;
            spriteClutY = g_FieldFxPaletteEntry2;
            asm("" : "=r"(params) : "0"(params), "r"(spriteClutY), "r"(spriteExtent));
            palettes = (RoomM256TrailPalettes *)&g_FieldFxTpageSlot2;
            asm("" : "=r"(palettes) : "0"(palettes));
            params->parameter00 = spriteExtent;
            spritePalette = palettes->first;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = spriteExtent;
            g_FieldFxSpriteParams.extent_y = spriteExtent;

            renderPalette = g_FieldFxTpageTable[spritePalette];
            D_800F336C = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = renderPalette;
            {
                register RoomM256TrailEffect *callState asm("$4");
                register void *callRotation asm("$5");
                register int callScaleX asm("$6");
                register int callScaleY asm("$7");
                int drawClut;
                int drawOffset;

                drawClut = GetClut(0x20, spriteClutY) & 0xFFFF;
                callState = state;
                callRotation = 0;
                callScaleX = renderScale;
                callScaleY = renderScale;
                drawOffset = (s16)((const RenderEffectParameters *)params)->parameter02 * 2 + 0xD8;
                /* Matching debt: consume CLUT and page in $2, then bound both
                 * outgoing stores before the texture-offset load. Empty ASM
                 * only; $4..$7 preserve the ready register arguments. */
                FieldFx_DrawShapeQuads((GteShortVector *)callState, callRotation, callScaleX, callScaleY,
                              drawOffset,
                    ({
                        register int clutWord asm("$2") = drawClut;
                        clutWord;
                    }),
                    ({
                        register int modeWord asm("$2") = 1;
                        modeWord;
                    }), firstFade,
                    (RenderColor *)(({
                        asm("" : : "r"(callState), "r"(callRotation),
                            "r"(callScaleX), "r"(callScaleY) : "$21", "$2");
                        &color.r;
                    })));
            }
            renderPalette = g_FieldFxTpageTable[palettes->second];

            D_800F336C = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = renderPalette;
            break;
        case 2:
            renderScale = rsin(((state->frame << 0xA) / 10)) / 4;
            fade = (rsin((state->frame << 0xB) / 10) / 48);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                int pageDepth = 0;
                int pageBlend = 1;
                int pageX = 0;
            paletteIndex = &g_FieldFxTpageSlot3;
            renderPalette = g_FieldFxTpageTable[*paletteIndex];

            D_800F336C = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = renderPalette;
            tpage = GetTPage(pageDepth, pageBlend, pageX, 0);
            }
            modelPage = (*(u16 *)&((char *)g_FieldFxTpageTable)[(unsigned)*paletteIndex << 1] | tpage) & 0xFFFF;
            modelClutY = g_FieldFxPaletteTable[D_800F336C];
            if (D_800F336C == 4) {
                if (g_FieldFxAltPaletteFlag != 0) {
                    modelClutY += 4;
                }
            }
            FieldFx_ModelSetTexture(modelPage, GetClut(0x70, modelClutY) & 0xFFFF);
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&state->motionX, &transform);
            transform.t[0] = state->x;
            transform.t[1] = state->y;
            translationZ = state->z;
            scale.x = renderScale;
            scale.y = renderScale;
            scale.z = 0x2AA;
            transform.t[2] = translationZ;
            ScaleMatrix(&transform, &scale.x);
            FieldFx_ModelSaveColors((char *)g_RoomM256TrailModel);
            FieldFx_ModelSetColors((char *)g_RoomM256TrailModel, darkColor.r, darkColor.g, darkColor.b);
            FieldFx_ModelScaleColors((char *)g_RoomM256TrailModel, fade & 0xFFFF);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM256TrailModel, &transform);
            FieldFx_ModelRestoreColors((char *)g_RoomM256TrailModel);
            return 0;
        case 3:
            pulseSine = rsin(state->frame << 6);
            renderScale = ((int) ((pulseSine >> 0x1F) + pulseSine) >> 1) + 0x1000;
            pulseCosine = rcos(state->frame << 6);
            fade = pulseCosine >> 5;
            if (pulseCosine < 0) {
                fade = (pulseCosine + 0x1F) >> 5;
            }
            pulseExtent = 0x40;
            g_FieldFxSpriteParams.parameter00 = pulseExtent;
            g_FieldFxSpriteParams.extent_x = pulseExtent;
            g_FieldFxSpriteParams.extent_y = pulseExtent;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.depth = 0x18;
            pulseClutY = g_FieldFxPaletteTable[D_800F336C];
            if ((D_800F336C == 4) && (g_FieldFxAltPaletteFlag != 0)) {
                pulseClutY += 4;
            }
            FieldFx_DrawShapeQuads((GteShortVector *)state, 0, renderScale, renderScale, 0x7C, GetClut(0x60, pulseClutY) & 0xFFFF, 1, fade, 0);
            g_FieldFxSpriteParams.depth = 8;
            break;
        case 4:
            renderScale = rsin(state->frame << 6) + 0x1000;
            rsin((state->frame << 11) / 24);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            impactClutY = g_FieldFxPaletteTable[D_800F336C];
            if ((D_800F336C == 4) && (g_FieldFxAltPaletteFlag != 0)) {
                impactClutY += 4;
            }
            clut = GetClut(0x10, impactClutY) & 0xFFFF;
            tripledScale = renderScale * 3;
            impactScale = (int) (tripledScale + (tripledScale >> 0x1F)) >> 1;
            spriteFrame = &g_FieldFxSpriteParams.parameter02;
            FieldFx_DrawShapeQuads((GteShortVector *)state, &spriteRotation, impactScale, impactScale, (*spriteFrame * (s16) (state->frame / 3)) + 0x20, clut, 1, 0x80, 0);
            impactFrame = state->frame;
            if (impactFrame < 0xD) {
                renderScale = rsin(((impactFrame << 10) / 12)) * 4;
                impactCosine = rcos(((state->frame << 10) / 12));
                fade = impactCosine >> 5;
                if (impactCosine < 0) {
                    fade = (impactCosine + 0x1F) >> 5;
                }
                g_FieldFxSpriteParams.parameter00 = 0x20;
                *spriteFrame = 2;
                g_FieldFxSpriteParams.extent_x = 0x20;
                g_FieldFxSpriteParams.extent_y = 0x20;
                g_FieldFxSpriteParams.parameter0A = 5;
                ringClutY = g_FieldFxPaletteTable[D_800F336C];
                if ((D_800F336C == 4) && (g_FieldFxAltPaletteFlag != 0)) {
                    ringClutY += 4;
                }
                FieldFx_DrawShapeQuads((GteShortVector *)state, &spriteRotation, renderScale, renderScale, 0x92, GetClut(0x80, ringClutY) & 0xFFFF, 1, fade, (RenderColor *)&darkColor.r);
                g_FieldFxSpriteParams.parameter0A = 0;
            }
            return 0;
        }
        break;
    default:
        break;
    }
    return 0;
}

int RoomM256_TrailEmitter(int mode, u16 *out) {
    FieldActor *actor;
    FieldActorState *state;
    u8 *action;
    /* Keep the return register available for the retail mode dispatch. */
    register int ret asm("$2");
    RoomM256TrailEffect *task;
    u16 value;

    switch (mode) {
    case 0:

        *out = (u16)g_FieldFxTaskContext->variables[0];
        g_RoomM256TrailModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5887704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM256TrailModel, 0, 0);
        if (g_FieldFxTaskContext->flags) {
            actor = g_FieldFxOwner->actor;
            if (actor) {
                state = actor->state;
                if (state) {
                    action = state->action_state;
                    if (*action == 1) *action = 2;
                }
            }
        }
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 0x18, 0x20,
                               RoomM256_TrailCallback);

        case 1:
    if (g_FieldFxCurrentAge == mode) {
        task = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
        if (task) {
            value = *out;
            task->kind = 0;
            task->frame = 0;
            task->index = value;
        }
        FieldFx_PlaySoundAtOwner(0x5A6, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5BE, 0x80);
    }
    ret = g_FieldFxCurrentAge < 8;
    if (ret) return 0;
    return 2;

    case 2:
    {
        unsigned palette_index = g_FieldFxTpageSlot3;
        u16 palette;
        palette = g_FieldFxTpageTable[palette_index];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

int RoomM256_ModelPulseParticleCallback(int mode, RoomModelPulseParticle *particle) {
    GteShortVector position;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    u16 clut;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 8;
        if ((s16)particle->frame >= 16) return 1;
        break;
    case 2:
        scale = particle->scale * rcos((s16)particle->frame << 6) / 4096;
        FieldFx_OffsetPointByAngles(&g_RoomM256PulseCenter, &position, scale, &g_RoomM256PulseAngles,
                      scale, (s16)particle->offset);
        FieldFx_SampleColorTrack(g_RoomM256PulseParticleColorTrack, &color, 0x30 - (scale * 48) / 1024);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x20, palette);
        FieldFx_DrawShapeQuads(&position, 0, 0x1000, 0x1000,
                       (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xD8, clut, 3,
                       g_RoomM256PulseBrightness, (RenderColor *)&color);
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        FieldFx_DrawHaloedPoint(&position, &color, g_RoomM256PulseBrightness, 1);
        break;
    }
    return 0;
}

/* Controller: loads the model asset, seeds sixteen ring particles on frame
 * 1, then draws the slot-2 ring sprite and, from frame 4, the model scaled by
 * cos and brightened by sin of the elapsed time.
 *
 * Mode 2 keeps the ModelBurstController preamble (spin record, brightness
 * scaled by 2/3, a cos sample) whose results this effect never reads: the
 * spin stores and the cos call survive in retail, and the brightness test is
 * the branch that splits the scheduling block before the /40 division (its
 * empty jump is only removed after scheduling). */
int RoomM256_ModelPulse(int mode, s16 *state) {
    GteRotation rotation = g_RoomM256PulseAnchor;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RoomModelPulseMatrix matrix;
    RoomModelPulseScale scale;
    RoomModelPulseParticle *child;
    void *slot;
    u16 *tpages;
    int size;
    int brightness;
    int i;

    switch (mode) {
    case 0:
        *state = 0;
        g_RoomM256PulseModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5487704);
        FieldFx_ModelScrollUv(g_RoomM256PulseModel, 0, 0);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 8, 0x10,
                             (FieldFxCallbackListCallback)RoomM256_ModelPulseParticleCallback);
    case 1:
        if (*state < 0x80) *state += 4;
        if (g_FieldFxCurrentAge == 1) {
            for (i = 0; i < 16; i++) {
                slot = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                child = slot;
                if (child) {
                    child->frame = 0;
                    child->scale = (rand() & 0x1FF) + 0x200;
                    child->offset = rand();
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x10) break;
        return 1;
    case 2:
        {
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x18;
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 2, &rotation, &position);
        spin.x = 0;
        spin.y = 0;
        spin.flags = 0;
        spin.z = -g_FieldFxCurrentAge << 6;
        /* Copied from the burst template; the value is never read here, but
         * the emptied branch still splits retail's schedule. */
        brightness = *state;
        if (g_FieldFxCurrentAge != 0) brightness = brightness * 2 / 3;
        rcos((g_FieldFxCurrentAge << 10) / 40);
        FieldFx_SampleColorTrack(g_RoomM256PulseColorTrack, &color, g_FieldFxCurrentAge * 3);
        FieldFx_DrawStarFan(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0,
                      0x80, 1);
        g_RoomM256PulseBrightness = *state;
        g_RoomM256PulseCenter.x = position.x;
        g_RoomM256PulseCenter.y = position.y;
        g_RoomM256PulseCenter.z = position.z;
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 2,
                      &g_RoomM256PulseAngles);
        g_RoomM256PulseAngles.x += 0x400;
        if (g_FieldFxCurrentAge >= 4) {
            int angle;

            i = g_FieldFxCurrentAge - 4;
            angle = (i << 10) / 12;
            size = rcos(angle);
            brightness = rsin(angle) / 32;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            {
                u16 clut;
                int kind;
                int palette;
                int page;
                int tpage;
                tpages = g_FieldFxTpageTable;
                tpage = tpages[g_FieldFxTpageSlot3];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
                page = (tpages[g_FieldFxTpageSlot3] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x50, palette);
                FieldFx_ModelSetTexture(page, clut);
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&g_RoomM256PulseAngles, &matrix);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_RoomM256PulseModel);
            FieldFx_ModelScaleColors(g_RoomM256PulseModel, (u16)brightness);
            FieldFx_ModelDraw(g_RoomM256PulseModel, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_RoomM256PulseModel);
        }
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        g_RoomM256PulseAngles.x += 0x600;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.depth = 0xC;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

void *RoomM256_ScatterConfigure(void) {
    return g_RoomM256ScatterConfig;
}

void *RoomM256_SetTrailTurnScale(void *arg0, int value) {
    if (value < 0x200) {
        value = 0x200;
    }

    g_RoomM256TrailTurnScale = value;
    return g_RoomM256TrailConfig;
}
