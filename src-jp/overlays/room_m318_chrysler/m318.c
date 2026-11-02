#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/room_ember_burst.h"
#include "pe1/room_ember_spiral.h"
#include "pe1/room_shake_burst.h"
#include "pe1/room_twin_ribbon.h"

#include "m318_externs.h"

typedef struct RoomM318Node {
    char pad[8];
    void *link;
} RoomM318Node;

typedef RoomTwinRibbonPulse RoomM318Pulse;

/* printf. */
int printf(const char *format, ...);

FieldActor *RoomM318_FindActorPosition(int type_id, int sub_id, s16 *position) {
    FieldActor *actor;

    actor = g_FieldActorListHead;
    while (actor != 0) {
        if ((actor != ((FieldActor *)D_8009CD10)) && (actor->state != 0) &&
            (actor->state->control10.command_value > 0) &&
            (actor->sub_id == type_id) && (actor->type_id == sub_id)) {
            FieldFx_GetActorPosition((char *)actor, 0, position);
            return actor;
        }
        actor = actor->next;
    }

    printf(g_RoomM318GetMayaPosErrorFormat, type_id, sub_id);
    return 0;
}

s32 RoomM318_FanSweepTrailCallback(s32 mode, s16 *pos) {
    s16 in[4];
    s16 out[4];
    switch (mode) {
    case 1:
    pos[0] = (u16)(pos[0] - 3 + (rand() & 7));
    pos[1] = (u16)(pos[1] - 7 + (rand() & 3));
    pos[2] = (u16)(pos[2] - 3 + (rand() & 7));
    if (g_FieldFxCurrentAge >= 0x12) {
        return 1;
    }
    break;
    case 2:
    in[0] = pos[0];
    in[1] = pos[1];
    in[2] = pos[2];
    FieldFx_SampleColorTrack(g_RoomM318FanSweepTrailColorTrack, out, g_FieldFxCurrentAge);
    FieldFx_DrawHaloedPoint(in, out, 0x80, 1);
    break;
    default:
        return 0;
    }
    return 0;
}

#define ROOM_FAN_SWEEP_SPARK_NAME RoomM318_FanSweepSpark
#define ROOM_FAN_SWEEP_SPARK_COLOR g_RoomM318FanSweepSparkColor
#define ROOM_FAN_SWEEP_SPARK_ANCHOR g_RoomM318FanSweepAnchor
#define ROOM_FAN_SWEEP_SPARK_TRAIL g_RoomM318FanSweepSparkPool
#define ROOM_FAN_SWEEP_SPARK_COS rcos
#define ROOM_FAN_SWEEP_SPARK_SIN rsin
#define ROOM_FAN_SWEEP_SPARK_RAND rand
#define ROOM_FAN_SWEEP_SPARK_PALETTE_LIT 9
#define ROOM_FAN_SWEEP_SPARK_PALETTE_BASE 5
#include "../room_lib/fan_sweep_spark.inc"

/* Fan sweep: attaches to the actor named by its parameters at floor level,
 * plays its sounds and opens a spark pool plus a trail pool; for 32 frames
 * every other frame it launches a spark on an angle that turns by a third
 * of a circle, updates the trail pool for 70 frames and each draw
 * publishes its anchor for the trail. */
int RoomM318_FanSweep(int mode, RoomFanSweep *sweep, RoomEmberBurstParams *params) {
    RoomFanSweepSpark *spark;
    int handles;

    switch (mode) {
    case 0:
        sweep->angle = rand();
        RoomEffect_AttachToActor(params->subId, params->typeId, sweep);
        sweep->y = g_RoomFloorY->y;
        FieldFx_PlaySoundAtOwner(0x5F3, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5F4, 0x80);
        handles = FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x10, 0x18,
                                (FieldFxCallbackListCallback)RoomM318_FanSweepSpark);
        handles += FieldFx_AllocCallbackList(&sweep->pool, handles, 8, 0x12, RoomM318_FanSweepTrailCallback);
        return handles;
    case 1:
        if (g_FieldFxCurrentAge < 0x20 && (g_FieldFxCurrentAge & 1)) {
            spark = (RoomFanSweepSpark *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
            if (spark) {
                spark->angle = sweep->angle;
                spark->radius = 0xC8;
                spark->state = 0;
                spark->timer = 0;
                sweep->angle -= 0x555;
            }
        }
        if (g_FieldFxCurrentAge >= 0x46) return 1;
        FieldFx_UpdateCallbackList(sweep->pool);
        g_RoomM318FanSweepSparkPool = sweep->pool;
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.depth = 8;
        FieldFx_DrawCallbackList(sweep->pool);
        g_RoomM318FanSweepAnchor.x = sweep->x;
        g_RoomM318FanSweepAnchor.y = sweep->y;
        g_RoomM318FanSweepAnchor.z = sweep->z;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

#undef ROOM_FAN_SWEEP_SPARK_NAME
#undef ROOM_FAN_SWEEP_SPARK_COLOR
#undef ROOM_FAN_SWEEP_SPARK_ANCHOR
#undef ROOM_FAN_SWEEP_SPARK_TRAIL
#undef ROOM_FAN_SWEEP_SPARK_COS
#undef ROOM_FAN_SWEEP_SPARK_SIN
#undef ROOM_FAN_SWEEP_SPARK_RAND
#undef ROOM_FAN_SWEEP_SPARK_PALETTE_LIT
#undef ROOM_FAN_SWEEP_SPARK_PALETTE_BASE

int RoomM318_SpiralDropSparkCallback(int mode, volatile RoomSpiralDropSparkRecord *record) {
    volatile RoomSpiralDropSparkCallbackView *particle = &record->callback;
    int color[2];
    GteShortVector position;
    switch (mode) {
    case 1: {
        s16 speed = particle->speed - 1;
        s16 y = particle->y;
        s16 delta = particle->speed;
        int frame = g_FieldFxCurrentAge;
        ((RoomSpiralDropSparkCallbackView *)particle)->y = y - delta;
        ((RoomSpiralDropSparkCallbackView *)particle)->speed = speed;
        if (frame >= 24) return 1;
        break;
    }
    case 2: {
        int palette;
        int kind;
        int z;
        unsigned short clut;
        FieldFx_SampleColorTrack(g_RoomM318SpiralDropSparkColorTrack, color, g_FieldFxCurrentAge);
        kind = g_FieldFxSpriteParams.palette;
        position.x = particle->x;
        position.y = particle->y;
        z = particle->z;
        position.z = z;
        palette = g_FieldFxPaletteTable[kind];
        clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 10 : palette + 6);
        FieldFx_DrawShapeQuads(&position, 0, 2048, 2048, 6, clut, 1, 128, (RenderColor *)color);
        break;
    }
    }
    return 0;
}

/* Spiral drop: attaches to the actor named by its parameters; for 51
 * frames it drops a falling spark from a point circling the actor on a
 * turning angle, while a glowing ring pulses there and sets the radius. */
int RoomM318_SpiralDrop(int mode, RoomSpiralDrop *drop, RoomEmberBurstParams *params) {
    RenderColor color;
    GteShortVector position;
    GteRotation spin;
    RoomSpiralDropSparkRecord *spark;
    int size;

    switch (mode) {
    case 0:
        drop->angle = rand();
        drop->radius = 0;
        RoomEffect_AttachToActor(params->subId, params->typeId, drop);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 8, 0x18,
                             (FieldFxCallbackListCallback)RoomM318_SpiralDropSparkCallback);
    case 1:
        if (g_FieldFxCurrentAge < 0x33) {
            spark = (RoomSpiralDropSparkRecord *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
            if (spark) {
                spark->emitter.x = drop->x + rcos(drop->angle) * drop->radius / 4096;
                spark->emitter.z = drop->z + rsin(drop->angle) * drop->radius / 4096;
                spark->emitter.y = drop->y;
                spark->emitter.speed = rand() & 3;
                drop->angle += 0x8AA + (rand() & 0x1F);
            }
        }
        if (g_FieldFxCurrentAge < 0x4A) break;
        return 1;
    case 2:
        g_FieldFxSpriteParams.depth = 8;
        if (g_FieldFxCurrentAge < 0x33) {
            position.x = drop->x;
            position.y = drop->y;
            position.z = drop->z;
            spin.x = 0x400;
            spin.y = 0;
            spin.z = g_FieldFxCurrentAge << 5;
            spin.flags = 1;
            size = rsin((g_FieldFxCurrentAge << 10) / 50);
            drop->radius = size / 8;
            FieldFx_SampleColorTrack(g_RoomM318SpiralDropColorTrack, &color, g_FieldFxCurrentAge);
            FieldFx_DrawRingBand(&position, 350, 500, 0x14, &spin, size, size, 0, &color,
                          0x80, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 4;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Sweep particle: while it rides the sweep centre it spins up, pulses its
 * size and drops a decaying copy every other frame; the draw turns and
 * stretches the shared model at its position. */
int RoomM318_ShakeSweepParticleCallback(int mode, RoomShakeSweepParticle *p) {
    GteShortVector rotation;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomShakeSweepParticle *child;
    int page;
    int spin;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 1:
        if (p->state == 0) {
            p->x = g_RoomM318ShakeSweepAnchor.x;
            p->z = g_RoomM318ShakeSweepAnchor.z;
            p->y = g_RoomM318ShakeSweepAnchor.y;
            spin = p->angle + 0x20;
            p->angle = spin + g_FieldFxCurrentAge * 4;
            p->size = rsin((g_FieldFxCurrentAge << 11) / 24) / 32;
            p->stretch = (g_FieldFxCurrentAge << 12) / 24;
            if (g_FieldFxCurrentAge & 1) {
                child = (RoomShakeSweepParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child != 0) {
                    child->state = 1;
                    child->x = p->x;
                    child->y = p->y;
                    child->z = p->z;
                    child->size = p->size;
                    child->stretch = p->stretch;
                }
            }
            if (g_FieldFxCurrentAge >= 0x18)
                return 1;
        } else {
            p->angle += 3;
            p->size = p->size * 3 / 4;
            if (g_FieldFxCurrentAge >= 4)
                return 1;
        }
        p->timer++;
        break;
    case 2:
        scale.z = 0x400;
        scale.x = 0x400;
        scale.y = p->stretch / 4;
        rotation.x = 0;
        rotation.y = p->angle;
        rotation.z = 0;
        RotMatrixYXZ(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = p->x;
        matrix.t[1] = p->y;
        matrix.t[2] = p->z;
        index = &g_FieldFxTpageSlot11;
        tpages = g_FieldFxTpageTable;
        {
            int tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        {
            int kind;
            int palette;
            u16 rawPage = GetTPage(0, 1, 0, 0);
            page = (u16)(tpages[*index] | rawPage);
            kind = D_800F2E2C;
            palette = g_FieldFxPaletteTable[kind];
            palette = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8 : palette + 4);
            FieldFx_ModelSetTexture(page, (u16)palette);
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_800F2D98);
        FieldFx_ModelSetColors(D_800F2D98, (u8)(p->size / 3), 0, (u8)(p->size / 2));
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F2D98, &matrix);
        FieldFx_ModelRestoreColors(D_800F2D98);
        break;
    }
    return 0;
}

/* Plays the sweep sounds and wakes the actor's status byte, then for 43
 * frames shakes the camera while handing a turning sweep angle to a new
 * particle every sixth frame; at frame 44 it hands the actor to the scene
 * script, and every draw publishes the anchor as the sweep centre. */
int RoomM318_ShakeSweep(int mode, RoomShakeSweepAnchor *anchor) {
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    RoomShakeSweepParticle *next;
    void **soundSlot;
    int volume;
    int time;

    switch (mode) {
    case 0:
        anchor->angle = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, (s16 *)anchor);
        anchor->y = g_RoomFloorY->y;
        soundSlot = g_LoadedSceneAssetBlock;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(*soundSlot, 0x5EF, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5F0, 0x80, 0x80, volume);
        }
        if (FX_TASK_AS(RoomShakeBurstEvent)->active) {
            pool = ANIM_OWNER_AS(RoomShakeBurstChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F2D98, 0, 0);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 0x10, 0x18,
                             (FieldFxCallbackListCallback)RoomM318_ShakeSweepParticleCallback);
    case 1:
        if (g_FieldFxCurrentAge < 0x2B)
            Battle_HaltOnPositiveX();
        if (g_FieldFxCurrentAge < 0x28 && g_FieldFxCurrentAge % 6 == 0) {
            next = (RoomShakeSweepParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (next != 0) {
                next->angle = anchor->angle;
                next->state = 0;
                next->timer = 0;
                {
                    int jitter = rand() & 0xFF;
                    int angle = anchor->angle - 0x555;
                    anchor->angle = angle - jitter;
                }
            }
        }
        if (g_FieldFxCurrentAge == 0x2B)
            Battle_InitEntityColors();
        if (g_FieldFxCurrentAge == 0x2C && FX_TASK_AS(RoomShakeBurstEvent)->active) {
            channel = ANIM_OWNER_AS(RoomShakeBurstChannel);
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*((RoomShakeBurstEntity **)D_8009CD10))->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (g_FieldFxCurrentAge < 0x40)
            break;
        return 1;
    case 2:
        g_RoomM318ShakeSweepAnchor.x = anchor->x;
        g_RoomM318ShakeSweepAnchor.y = anchor->y;
        g_RoomM318ShakeSweepAnchor.z = anchor->z;
        break;
    }
    return 0;
}

int RoomM318_ArcingSparkCallback(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (g_FieldFxCurrentAge < 19)
            state->velocity_y -= 1;
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->y + state->x + (g_FieldFxCurrentAge << 5);
        rotation.flags = 0;
        intensity = rsin((g_FieldFxCurrentAge << 11) / 24) / 128;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, 4096, 4096,
                     96 + (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 6),
                     GetClut(16, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int RoomM318_ArcingSparks(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->x);
        state->y = g_RoomFloorY->y;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 12, (FieldFxTaskCallback)RoomM318_ArcingSparkCallback);
    case 1:
        if (g_FieldFxCurrentAge < 40 && (g_FieldFxCurrentAge & 1)) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x + rand() % 400 - 200;
                particle->y = state->y - 800;
                particle->z = state->z + rand() % 400 - 200;
                particle->velocity_y = (rand() & 7) + 42;
            }
        }
        if (g_FieldFxCurrentAge >= 70)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 32;
        break;
    }
    return 0;
}

int RoomM318_StarFlare(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = g_RoomM318StarFlarePhase;
        g_RoomM318StarFlarePhase += 1365;
        FieldFx_GetActorPosition(ANIM_OWNER_AS(RoomM318Node)->link, 0, &state->position.x);
        state->position.y -= 100;
        state->position.x += rcos(state->phase) * 300 / 4096;
        state->position.z += rsin(state->phase) * 300 / 4096;
        return 0;
    case 1:
        if (g_FieldFxCurrentAge >= 8) return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 2048 + (g_FieldFxCurrentAge << 9);
        FieldFx_SampleColorTrack(g_RoomM318StarFlareColorTrackA, &color, g_FieldFxCurrentAge);
        FieldFx_DrawStarFan(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        FieldFx_DrawRingBand(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        FieldFx_SampleColorTrack(g_RoomM318StarFlareColorTrackB, &color, g_FieldFxCurrentAge);
        FieldFx_DrawStarFan(&position, 500, 100, 16, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int RoomM318_ShakeBurstParticleCallback(int mode, RoomShakeBurstParticleCallbackView *particle) {
    int color[2];
    GteShortVector position;
    int kind, palette, scale, frame, random, value;
    u16 clut;
    switch (mode) {
    case 1:
        random = rand();
        value = particle->x - 3;
        value += (random & 7);
        particle->x = value;
        random = rand();
        value = particle->z - 3;
        value += (random & 7);
        particle->z = value;
        random = rand();
        particle->y += random & 3;
        if (g_FieldFxCurrentAge >= 32) return 1;
        break;
    case 2:
        position.x = 0;
        position.y = 0;
        position.z = g_FieldFxCurrentAge * 12;
        position.pad = 0;
        FieldFx_SampleColorTrack(g_RoomM318ShakeBurstParticleColorTrack, color, g_FieldFxCurrentAge);
        kind = g_FieldFxSpriteParams.palette;
        scale = g_FieldFxCurrentAge * 128 + 4096;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(48, palette);
        frame = (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 6 + 2) + 160;
        FieldFx_DrawShapeQuads((GteShortVector *)particle, (GteRotation *)&position, scale, scale, frame, clut, 1, 128, (RenderColor *)color);
        break;
    }
    return 0;
}

/* Wakes the actor's status byte and plays the burst sounds, then for 39
 * frames shakes the camera while scattering particles around the anchor
 * for the first 21; at frame 40 it hands the actor to the scene script. */
int RoomM318_ShakeBurst(int mode, RoomShakeBurstPoint *anchor) {
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    RoomShakeBurstPoint *next;
    void **soundSlot;
    int volume;
    int time;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, (s16 *)anchor);
        if (FX_TASK_AS(RoomShakeBurstEvent)->active) {
            pool = ANIM_OWNER_AS(RoomShakeBurstChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = g_LoadedSceneAssetBlock;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(*soundSlot, 0x5F1, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5F2, 0x80, 0x80, volume);
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 8, 0x14,
                             (FieldFxCallbackListCallback)RoomM318_ShakeBurstParticleCallback);
    case 1:
        if (g_FieldFxCurrentAge < 0x27)
            Battle_HaltOnPositiveX();
        if (g_FieldFxCurrentAge < 0x15) {
            next = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (next != 0) {
                next->x = anchor->x + (rand() & 0x1FF) - 0x100;
                next->y = anchor->y + (rand() & 0x1FF) - 0x100;
                next->z = anchor->z + (rand() & 0x1FF) - 0x100;
            }
        }
        if (g_FieldFxCurrentAge == 0x27)
            Battle_InitEntityColors();
        if (g_FieldFxCurrentAge == 0x28 && FX_TASK_AS(RoomShakeBurstEvent)->active) {
            channel = ANIM_OWNER_AS(RoomShakeBurstChannel);
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                (*((RoomShakeBurstEntity **)D_8009CD10))->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (g_FieldFxCurrentAge < 0x35)
            break;
        return 1;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x10;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Particle-only declarations shared with the shake burst renderer. */

/* Ember burst particle: embers drift up and bounce off the floor, flashes
 * pulse in place, and orbiting sparks sweep around their anchor drawing
 * the glow track; smoke rises and fades. */
int RoomM318_EmberBurstParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_RoomM318ZeroSpin;
    GteShortVector offset;
    RenderColor color = g_RoomM318EmberParticleColor;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            p->y -= 8;
            if ((s16)p->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = rsin(angle) / 2 + 0x800;
            glow = rcos(angle) / 32;
            spin.z = -((s16)p->timer * 32);
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size, 0xDC,
                          GetClut(0x30, palette), 1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            FieldFx_AnglesToVector(&p->heading,
                          (s16)(rcos(angle) * p->radius / 4096), &offset);
            offset.x += p->x;
            offset.y += p->y;
            offset.z += p->z;
            size = rsin(angle);
            spin.z = (s16)p->timer * 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberBurstParticleColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&offset, &spin, size / 2, size / 2, 0x24,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                    : palette + 4),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = rsin(angle) + 0x1000;
            glow = rcos(angle) / 32;
            spin.z = -((s16)p->timer * 32);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size, 0x42,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember burst: glows up for 16 frames, then for 24 frames scatters
 * orbiting sparks, embers on odd frames and smoke on even ones, then fades;
 * while it glows it draws a flickering halo and a ring. */
int RoomM318_EmberBurst(int mode, RoomEmberBurst *burst, RoomEmberBurstParams *params) {
    RenderColor ringColor = g_RoomM318EmberRingColor;
    RenderColor haloColor = g_RoomM318EmberHaloColor;
    RoomOrbitTrailParticle *child;
    int glow;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        RoomEffect_AttachToActor(params->subId, params->typeId, burst);
        FieldFx_PlaySoundAtOwner(0x5F5, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5F6, 0x80);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)RoomM318_EmberBurstParticleCallback);
    case 1:
        switch (burst->state) {
        case 0:
            burst->timer++;
            burst->glow = rsin(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            if (burst->timer < 0x19) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->x += (rand() & 0x3F) - 0x20;
                    child->y += (rand() & 0x3F) - 0x20;
                    child->z += (rand() & 0x3F) - 0x20;
                    child->radius = (rand() & 0x1FF) + 0x200;
                    child->heading.x = rand();
                    child->heading.y = rand();
                    child->heading.z = rand();
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (burst->timer & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->y += 0x100 - (rand() & 0x1FF);
                    child->x += (rand() & 0xFF) - 0x80;
                    child->z += (rand() & 0xFF) - 0x80;
                    child->heading.x = rand() % 32 - 0x10;
                    child->heading.y = rand() % 8 - 4;
                    child->heading.z = rand() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (!(burst->timer & 1)) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->x += (rand() & 0x1FF) - 0x100;
                    child->y += (rand() & 0x1FF) - 0x100;
                    child->z += (rand() & 0x1FF) - 0x100;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (burst->timer < 0x20) break;
            burst->state = 2;
            burst->timer = 0;
            break;
        case 2:
            burst->timer++;
            burst->glow = rcos((burst->timer << 10) / 24) / 32;
            if (burst->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (burst->glow != 0) {
            RenderColor *color;
            int scale;
            glow = burst->glow;
            color = (g_FieldFxCurrentAge & 1) ? &ringColor : &haloColor;
            scale = 0x1000;
            g_FieldFxSpriteParams.depth = 0x28;
            FieldFx_DrawStarFan((GteShortVector *)burst, 400, 400, 0x10, 0, scale, scale,
                          color, 0, glow, 1);
            FieldFx_DrawRingBand((GteShortVector *)burst, 500, 700, 0x18, 0, scale, scale, 0,
                          &ringColor, glow / 2, 3);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Rising column particle: rings pulse at the column centre while sparks
 * drift and bounce on the floor; both draw with the column's glow track. */
int RoomM318_RisingColumnParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_RoomM318FlatTilt;
    RenderColor color;
    int fall;
    int bounce;
    int angle;
    int size;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = rsin(angle) * 5 / 2;
            angle = rcos(angle) / 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            spin.z += g_FieldFxCurrentAge << 5;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot10];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318RisingColumnParticleColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&g_RoomM318RisingColumnAnchor, &spin, size, size, 0x44,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                    : palette + 4),
                          1, angle, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = rcos(angle) / 2 + 0x800;
            angle = rcos(angle) / 32;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318RisingColumnParticleColorTrack, &color, (s16)p->timer * 2);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, 0, size / 2, size / 2,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xD8,
                          GetClut(0x20, palette), 1, angle, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Rising column: follows the actor while its height eases from the floor
 * over the configured duration, shedding a falling spark every frame and
 * a ring every fourth; the draw publishes it as the column centre. */
int RoomM318_RisingColumn(int mode, RoomRisingColumn *column,
                  RoomRisingColumnParams *params) {
    RoomOrbitTrailParticle *child;
    int rise;

    switch (mode) {
    case 0:
        column->counter = 0;
        column->timer = 0;
        FieldFx_PlaySoundAtOwner(0x5EA, FieldFx_GetOwnerProgress());
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 0x14, 0x20,
                             (FieldFxCallbackListCallback)RoomM318_RisingColumnParticleCallback);
    case 1:
        column->timer++;
        FieldFx_GetActorPosition((char *)ANIM_OWNER_AS(RoomShakeBurstChannel)->pool, 0, (s16 *)column);
        column->y = g_RoomFloorY->y - params->offset;
        rise = params->height * column->timer / params->duration;
        if (params->descend)
            column->y -= rise;
        else
            column->y -= params->height - rise;
        if (column->timer <= params->duration) {
            if ((column->timer & 3) == 0) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (child) {
                child->x = column->x;
                child->y = column->y;
                child->z = column->z;
                child->x += (rand() & 0xFF) - 0x80;
                child->z += (rand() & 0xFF) - 0x80;
                child->heading.x = rand() % 32 - 0x10;
                /* The vertical heading spends a random draw on a zero range. */
                child->heading.y = rand() % 1;
                child->heading.z = rand() % 32 - 0x10;
                child->state = 1;
                child->timer = 0;
            }
        }
        if (column->timer < params->duration + 8) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_RoomM318RisingColumnAnchor.x = column->x;
        g_RoomM318RisingColumnAnchor.y = column->y;
        g_RoomM318RisingColumnAnchor.z = column->z;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Flash burst particle: drifting rings slow down, falling sparks slow down
 * and bounce on the floor; both draw as spinning glow sprites that swell
 * and fade over 24 frames. */
int RoomM318_FlashBurstParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = rsin(angle) + 0x800;
            size = size * p->radius / 4096;
            glow = rcos(angle) / 32 + 0x28;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size, 6,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = rsin(angle) + 0x1000;
            glow = rsin(angle * 2) / 32;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 14;
            spin.flags = 0;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            palette = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6
                                                                : palette + 2);
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size * 3, size * 3, 0,
                          (u16)palette, 1, glow, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Flash burst: while the scene event runs it plays the burst sounds, drops
 * falling sparks at frame 3 and two drifting rings up to frame 4, then
 * fades a glow sprite, halo and ring over 32 frames. */
int RoomM318_FlashBurst(int mode, RoomShakeBurstPoint *burst) {
    RenderColor glowColor;
    RenderColor color = g_RoomM318FlashBurstColor;
    RoomOrbitTrailParticle *child;
    void **soundSlot;
    int volume;
    int time;
    int i;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, (s16 *)burst);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 0x14, 0x10,
                             (FieldFxCallbackListCallback)RoomM318_FlashBurstParticleCallback);
    case 1:
        if (FX_TASK_AS(RoomShakeBurstEvent)->running == 0)
            return 1;
        if (g_FieldFxCurrentAge == 2) {
            soundSlot = g_LoadedSceneAssetBlock;
            if (*soundSlot != 0) {
                volume = 0x7F;
                time = FieldFx_GetOwnerProgress();
                Akao_SendTableCommand(*soundSlot, 0x5E7, time, 0x80, volume);
                if (*(void *volatile *)soundSlot != 0)
                    Akao_SendTableCommand(*soundSlot, 0x5E8, 0x80, 0x80, volume);
            }
        }
        if (g_FieldFxCurrentAge == 3) {
            for (i = 0; i < 3; i++) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand() % 50 - 0x19;
                    child->heading.y = rand() % 50 - 0x19;
                    child->heading.z = rand() % 50 - 0x19;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 5) {
            for (i = 0; i < 2; i++) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->radius = (rand() & 0x7FF) + 0x800;
                    child->x += (rand() & 0x7F) - 0x40;
                    child->y += (rand() & 0x7F) - 0x40;
                    child->z += (rand() & 0x7F) - 0x40;
                    child->heading.x = rand() % 190 - 0x5F;
                    child->heading.y = rand() % 190 - 0x5F;
                    child->heading.z = rand() % 190 - 0x5F;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 8) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x20;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (g_FieldFxCurrentAge == 1)
            FieldFx_DrawScreenTint((unsigned char *)&color, 0x80, 2, 8);
        else if (g_FieldFxCurrentAge == 2)
            FieldFx_DrawScreenTint((unsigned char *)&color, 0x80, 3, 8);
        if (g_FieldFxCurrentAge < 0x21) {
            int clut;
            int kind;
            int palette;
            int angle = g_FieldFxCurrentAge << 5;
            int glow = rcos(angle) / 32;
            int scale;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x30, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 4) + 0xA0,
                          clut, 1, glow, 0);
            FieldFx_SampleColorTrack(g_RoomM318FlashBurstColorTrack, &glowColor, g_FieldFxCurrentAge);
            scale = rsin(angle) + 0x1000;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 2 / 3;
            FieldFx_DrawStarFan((GteShortVector *)burst, 500, 500, 10, 0, scale, scale,
                          &glowColor, 0, glow, 1);
            glow = rcos(angle) / 32;
            scale = rsin(angle) + 0x800;
            FieldFx_DrawRingBand((GteShortVector *)burst, 0x44C, 0x6A4, 0x10, 0, scale,
                          scale, 0, &glowColor, glow / 2, 1);
        }
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x10;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Cross flash particle: the flash draws as a pulsing tilted ring, the
 * sparks spin their orientation by their offset and draw as flickering
 * beams from the cross flash centre. */
int RoomM318_CrossFlashParticleCallback(int mode, RoomCrossFlashParticle *p) {
    GteShortVector origin;
    RenderColor glowColor;
    RenderColor color = g_RoomM318EmberParticleColor;
    int angle;
    int size;
    int glow;
    int length;
    int width;
    int u;
    int v;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->rotation.x += p->x;
            p->rotation.y += p->y;
            p->rotation.z += p->z;
            if ((s16)p->timer < 0x28) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = rsin((s16)p->timer << 5);
            FieldFx_SampleColorTrack(g_RoomM318CrossFlashParticleColorTrack, &glowColor, (s16)p->timer);
            FieldFx_DrawRingBand((GteShortVector *)p, 0x578, 0x898, 0xC, &p->rotation, size,
                          size, 0, &glowColor, 0x80, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 40;
            glow = rcos(angle) / 32;
            if (p->timer & 1)
                glow = glow * 2 / 3;
            origin.x = g_RoomM318CrossFlashAnchor.x;
            origin.y = g_RoomM318CrossFlashAnchor.y;
            origin.z = g_RoomM318CrossFlashAnchor.z;
            length = 0x76C;
            if ((s16)p->timer < 0x21)
                length = rsin((s16)p->timer << 5) * 1900 / 4096;
            width = rcos(angle) / 64 + 0x20;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            /* The flicker frame reuses the flash size temporary. */
            size = (s16)(p->timer & 3);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            u = (size & 1) * 128;
            v = (size / 2) * 16 + 0x80;
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawRotatedQuad(&origin, &p->rotation, length, width, u, v, 0x80, 0x10,
                          GetClut(0x50, palette), &color, &color, (s16)glow, 1);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Cross flash: takes the actor's orientation, then at frame 2 drops a
 * flash and four sparks turned a quarter turn about one axis, and at
 * frame 8 the same about the other axis; the draw publishes the anchor. */
int RoomM318_CrossFlash(int mode, RoomCrossFlash *flash) {
    RoomCrossFlashParticle *child;
    int i;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, (s16 *)flash);
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomShakeBurstChannel)->pool, 0,
                      (GteShortVector *)&flash->rotation);
        flash->rotation.x = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)RoomM318_CrossFlashParticleCallback);
    case 1:
        if (FX_TASK_AS(RoomShakeBurstEvent)->running == 0)
            return 1;
        if (g_FieldFxCurrentAge == 2) {
            child = (RoomCrossFlashParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (child) {
                child->x = flash->x;
                child->y = flash->y;
                child->z = flash->z;
                child->rotation.x = flash->rotation.x;
                child->rotation.y = flash->rotation.y;
                child->rotation.z = flash->rotation.z;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 4; i++) {
                child = (RoomCrossFlashParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->x = (rand() & 0x7F) - 0x40;
                    child->y = 0;
                    child->z = (rand() & 0x1F) + 0x10;
                    child->rotation.x = flash->rotation.x;
                    child->rotation.y = flash->rotation.y;
                    child->rotation.z = flash->rotation.z;
                    child->rotation.y += 0x400;
                    child->rotation.x += rand();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge == 8) {
            child = (RoomCrossFlashParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (child) {
                child->x = flash->x;
                child->y = flash->y;
                child->z = flash->z;
                child->rotation.x = flash->rotation.x;
                child->rotation.y = flash->rotation.y;
                child->rotation.z = flash->rotation.z;
                child->rotation.x += 0x400;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 4; i++) {
                child = (RoomCrossFlashParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->x = 0;
                    child->y = (rand() & 0x7F) - 0x40;
                    child->z = (rand() & 0x1F) + 0x10;
                    child->rotation.x = flash->rotation.x;
                    child->rotation.y = flash->rotation.y;
                    child->rotation.z = flash->rotation.z;
                    child->rotation.y += rand();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 8) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_RoomM318CrossFlashAnchor.x = flash->x;
        g_RoomM318CrossFlashAnchor.y = flash->y;
        g_RoomM318CrossFlashAnchor.z = flash->z;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x20;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

int RoomM318_TwinRibbonPulseCallback(int mode, RoomM318Pulse *pulse) {
    int angle, sine, cosine, size, clut, random;
    s16 value;
    s16 *settings;
    int clutX;
    register int clutY asm("$5");
    int width;
    int paletteIndex;
    switch (mode) {
    case 1: {
        if (pulse->delay) return 0;
        ++pulse->age;
        random = rand();
        value = pulse->y - 5;
        pulse->y = value - (random & 7);
        random = rand();
        value = pulse->x - 3;
        pulse->x = value + (random & 7);
        random = rand();
        value = pulse->z - 3;
        pulse->z = value + (random & 7);
        if (pulse->age >= 6) return 1;
        break;
    }
    case 2: {
        if (pulse->delay) return 0;
        angle = (pulse->age << 10) / 6;
        sine = rcos(angle);
        /* Round the first sine toward zero before the second lookup. */
        if (sine < 0) sine += 31;
        cosine = rcos(angle);
        clutX = 16;
        asm("" : "=r"(clutX) : "0"(clutX));
        settings = (s16 *)&g_FieldFxSpriteParams;
        asm("" : "=r"(settings) : "0"(settings));
        clutY = g_FieldFxPaletteEntry3;
        width = 32;
        settings[0] = width;
        paletteIndex = g_FieldFxTpageSlot3;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = width;
        g_FieldFxSpriteParams.extent_y = width;
        {
            int tpage = g_FieldFxTpageTable[paletteIndex];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        clut = GetClut(clutX, clutY);
        size = cosine / 2;
        FieldFx_DrawShapeQuads((GteShortVector *)pulse, 0, size, size, 104 + settings[1] * (pulse->age & 3), clut, 1, (sine >> 5) / 2, 0);
    }
    }
    return 0;
}

/* Twin ribbon controller: two mirrored pairs of ribbons between bone
 * offsets on bones 8 and 21; every frame a pulse is shed from one of the
 * two parameter bones. Draws a glow at the first offset, the ribbons and
 * restores the parameter block. */
int RoomM318_TwinRibbon(int mode, RoomTwinRibbon *ribbon, RoomTwinRibbonParams *params) {
    GteShortVector from;
    GteShortVector to;
    GteShortVector root = g_RoomM318TwinRibbonRoot;
    GteShortVector tip = g_RoomM318TwinRibbonTip;
    GteShortVector fork = g_RoomM318TwinRibbonFork;
    GteShortVector shed = g_RoomM318TwinRibbonShed;
    RenderColor colorA = g_RoomM318TwinRibbonColorA;
    RenderColor colorB = g_RoomM318TwinRibbonColorB;
    RenderColor colorC = g_RoomM318TwinRibbonColorC;
    RoomOrbitTrailParticle *pulse;
    int scale;
    int alpha;

    switch (mode) {
    case 0:
        ribbon->boneA = 6;
        ribbon->boneB = 0x13;
        ribbon->reserved04 = 0;
        ribbon->timer = 0;
        tip.x *= -1;
        fork.x *= -1;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &root, &from);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &tip, &to);
        FieldFx_DrawHistoryTrail(&from, &to, 0x3EC, (unsigned char *)&colorA, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailA, 1);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &fork, &from);
        FieldFx_DrawHistoryTrail(&from, &to, 0x3EE, (unsigned char *)&colorC, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailC, 1);
        tip.x *= -1;
        fork.x *= -1;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &root, &from);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &tip, &to);
        FieldFx_DrawHistoryTrail(&from, &to, 0x3EC, (unsigned char *)&colorA, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailB, 1);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &fork, &from);
        FieldFx_DrawHistoryTrail(&from, &to, 0x3EE, (unsigned char *)&colorC, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailD, 1);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x14, 0x10,
                             (FieldFxCallbackListCallback)RoomM318_TwinRibbonPulseCallback);
    case 1:
        ribbon->timer++;
        if (params->stop) return 2;
        if (!(ribbon->timer & 1))
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, ribbon->boneA, &shed, &from);
        else
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, ribbon->boneB, &shed, &from);
        pulse = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
        if (!pulse) return 0;
        pulse->x = from.x;
        pulse->y = from.y;
        pulse->z = from.z;
        pulse->state = 0;
        pulse->timer = 0;
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        alpha = 0x8C;
        scale = 0x1000;
        if (g_FieldFxCurrentAge & 1) {
            scale = 0x2000;
            alpha = 0x64;
        }
        g_FieldFxSpriteParams.parameter00 = 0x40;
        g_FieldFxSpriteParams.parameter02 = 4;
        g_FieldFxSpriteParams.extent_x = 0x40;
        g_FieldFxSpriteParams.extent_y = 0x40;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x30;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        root.z = 0;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0, &root, &from);
        {
            u16 clut;
            int kind;
            int palette;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            clut = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6 : palette + 2);
            FieldFx_DrawShapeQuads(&from, 0, scale * 3 >> 1, scale * 3 >> 1, 0, clut, 3, alpha, 0);
        }
        tip.x *= -1;
        fork.x *= -1;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &root, &from);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &tip, &to);
        FieldFx_DrawHistoryTrail(&from, &to, 4, (unsigned char *)&colorA, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailA, 1);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 8, &fork, &from);
        FieldFx_DrawHistoryTrail(&from, &to, 6, (unsigned char *)&colorC, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailC, 1);
        tip.x *= -1;
        fork.x *= -1;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &root, &from);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &tip, &to);
        FieldFx_DrawHistoryTrail(&from, &to, 4, (unsigned char *)&colorA, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailB, 1);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0x15, &fork, &from);
        FieldFx_DrawHistoryTrail(&from, &to, 6, (unsigned char *)&colorC, (unsigned char *)&colorB, 0x80, (struct FieldTrailPair *)ribbon->trailD, 1);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.depth = 0x10;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Ember fountain particle: embers fly out and bounce on the floor, orbiting
 * sparks ride the fountain centre while their heading turns, and smoke
 * rises; each draws with the fountain's glow track. */
int RoomM318_EmberFountainParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_RoomM318ZeroSpin;
    GteShortVector offset;
    RenderColor color;
    RenderColor seed = g_RoomM318EmberFountainParticleColor;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 50 / 51;
            p->heading.z = p->heading.z * 50 / 51;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x = g_RoomM318EmberFountainAnchor.x;
            p->y = g_RoomM318EmberFountainAnchor.y;
            p->z = g_RoomM318EmberFountainAnchor.z;
            p->heading.x += 0x1D;
            p->heading.y += 0xB;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            p->y -= 1;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            size = rcos(angle);
            glow = rsin(angle) / 32;
            spin.z = -((s16)p->timer * 24);
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberFountainParticleColorTrack, &color, (s16)p->timer * 24 / 16);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size, 0xD8,
                          GetClut(0x20, palette), 1, glow, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            FieldFx_AnglesToVector(&p->heading,
                          (s16)(rcos(angle) * p->radius / 4096), &offset);
            offset.x += p->x;
            offset.y += p->y;
            offset.z += p->z;
            size = rsin(angle) * 3 / 2;
            spin.z = (s16)p->timer * 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberFountainParticleColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&offset, &spin, size, size, 6,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                    : palette + 3),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = rcos(angle) + 0x1000;
            angle = rsin(angle * 2);
            glow = angle / 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, 0, size, size, 0x42,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                    : palette + 4),
                          1, glow / 2, &seed);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember fountain: follows its actor for 141 frames, swelling a glow; for
 * the first 134 frames it sprays orbiting sparks, embers on odd frames and
 * smoke every fourth. It publishes its position as the fountain centre and
 * draws a halo and ring at it. */
int RoomM318_EmberFountain(int mode, RoomEmberFountain *fountain) {
    GteShortVector position;
    RenderColor color = g_RoomM318EmberRingColor;
    RoomOrbitTrailParticle *child;
    int angle;
    int glow;

    switch (mode) {
    case 0:
        fountain->reserved08 = 0;
        fountain->timer = 0;
        fountain->glow = 0;
        fountain->ringScale = 0;
        FieldFx_PlaySoundAtOwner(0x5EB, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5EC, 0x80);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)RoomM318_EmberFountainParticleCallback);
    case 1:
        fountain->timer++;
        FieldFx_GetActorPosition((char *)ANIM_OWNER_AS(RoomEmberFountainChannel)->pool, 0, (s16 *)fountain);
        position.x = fountain->x;
        position.y = fountain->y;
        position.z = fountain->z;
        angle = (fountain->timer << 10) / 141;
        fountain->glow = rsin(angle) / 32;
        fountain->ringScale = rcos(angle);
        if (fountain->timer < 0x86) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (rand() & 0x3F) - 0x20;
                child->y += (rand() & 0x3F) - 0x20;
                child->z += (rand() & 0x3F) - 0x20;
                child->radius = (rand() & 0x3FF) + 0x3E8;
                child->heading.x = rand();
                child->heading.y = rand();
                child->heading.z = rand();
                child->state = 1;
                child->timer = 0;
            }
            if (fountain->timer & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->y += 0x100 - (rand() & 0x1FF);
                    child->x += (rand() & 0x1FF) - 0x100;
                    child->z += (rand() & 0x1FF) - 0x100;
                    child->heading.x = rand() % 32 - 0x10;
                    child->heading.y = rand() % 32 - 0x10;
                    child->heading.z = rand() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (!(fountain->timer & 3)) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->x += (rand() & 0x1FF) - 0x100;
                    child->y += (rand() & 0x1FF) - 0x100;
                    child->z += (rand() & 0x1FF) - 0x100;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        g_RoomM318EmberFountainAnchor.x = fountain->x;
        g_RoomM318EmberFountainAnchor.y = fountain->y;
        g_RoomM318EmberFountainAnchor.z = fountain->z;
        if (fountain->timer < 0x8D) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        position.x = fountain->x;
        position.y = fountain->y;
        position.z = fountain->z;
        if (fountain->timer < 0x8E) {
            glow = fountain->glow;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            g_FieldFxSpriteParams.depth = 0x28;
            FieldFx_DrawStarFan(&position, 1000, 1000, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            FieldFx_DrawRingBand(&position, 0x578, 0x708, 0x18, 0, fountain->ringScale,
                          fountain->ringScale, 0, &color, glow / 2, 1);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Twisting ember particle: embers fly out and bounce on the floor, sparks
 * twist along a bent trail, and smoke rises; each draws with the glow
 * track. */
int RoomM318_EmberSpiralParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_RoomM318ZeroSpin;
    GteShortVector offset;
    RenderColor color;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 50 / 51;
            p->heading.z = p->heading.z * 50 / 51;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->heading.pad += 0x32;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            p->y -= 1;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = rsin((s16)p->timer << 6) / 2 + 0x1000;
            spin.z = -((s16)p->timer * 32);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
                g_FieldFxSpriteParams.palette = 2;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberSpiralColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)p->timer / 2) + 0xA0,
                          GetClut(0x30, palette), 1, 0x80, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = (s16)p->timer << 6;
            FieldFx_OffsetPointByAngles(p, &offset, 200, &p->heading,
                          rsin(angle) * p->radius / 4096, p->heading.pad);
            size = rcos(angle) * 3 / 2;
            spin.z = p->heading.pad + (s16)p->timer * 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberSpiralColorTrack, &color, (s16)p->timer * 3 / 2);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&offset, &spin, size, size, 4,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                    : palette + 3),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 24;
            size = rcos(angle) + 0x1000;
            glow = rcos(angle) / 32;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.depth = 0x20;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_RoomM318EmberSpiralColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, 0, size, size, 0,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6
                                                                    : palette + 2),
                          1, glow / 2, &color);
            g_FieldFxSpriteParams.depth = 4;
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember spiral: plays its sounds, loads the spiral model and attaches to
 * the actor's hand; for 24 frames it sheds orbiting sparks along its
 * heading, embers on odd frames and smoke, hits the player in reach, and
 * draws a glow plus the spiral model swelling and turning in front of the
 * hand. */
int RoomM318_EmberSpiral(int mode, RoomEmberSpiral *spiral) {
    GteShortVector position;
    GteShortVector tip;
    GteShortVector rotation;
    RenderColor color;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomOrbitTrailParticle *child;
    RoomShakeBurstSlot *pool;
    RoomShakeBurstObject *object;
    RoomShakeBurstChannel *channel;
    void **soundSlot;
    u16 *index;
    u16 *tpages;
    int volume;
    int time;
    int glow;
    int width;
    int depth;
    int page;

    switch (mode) {
    case 0:
        spiral->reserved08 = 0;
        spiral->timer = 0;
        spiral->glow = 0;
        soundSlot = g_LoadedSceneAssetBlock;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(*soundSlot, 0x5ED, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5EE, 0x80, 0x80, volume);
        }
        g_RoomM318EmberSpiralModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5509704);
        FieldFx_ModelScrollUv(g_RoomM318EmberSpiralModel, 0, 0);
        if (FX_TASK_AS(RoomShakeBurstEvent)->active) {
            pool = ANIM_OWNER_AS(RoomShakeBurstChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomShakeBurstChannel)->pool, 0,
                      (GteShortVector *)spiral);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)RoomM318_EmberSpiralParticleCallback);
    case 1:
        spiral->timer++;
        FieldFx_GetActorPosition((char *)ANIM_OWNER_AS(RoomShakeBurstChannel)->pool, 0, (s16 *)&position);
        spiral->glow = rcos((spiral->timer << 10) / 24) / 32;
        if (spiral->timer < 0x19) {
            child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (rand() & 0x3F) - 0x20;
                child->y += (rand() & 0x3F) - 0x20;
                child->z += (rand() & 0x3F) - 0x20;
                child->radius = (rand() & 0x1FF) + 0x200;
                child->heading.x = spiral->x;
                child->heading.y = spiral->y;
                child->heading.z = spiral->z;
                child->heading.pad = rand();
                child->state = 1;
                child->timer = 0;
            }
            if (spiral->timer & 1) {
                child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->y += 0x100 - (rand() & 0x1FF);
                    child->x += (rand() & 0x1FF) - 0x100;
                    child->z += (rand() & 0x1FF) - 0x100;
                    child->heading.x = rand() % 32 - 0x10;
                    child->heading.y = rand() % 32 - 0x10;
                    child->heading.z = rand() % 32 - 0x10;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            child = (RoomOrbitTrailParticle *)FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomShakeBurstSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->x += (rand() & 0x1F) - 0x10;
                child->y += (rand() & 0x1F) - 0x10;
                child->z += (rand() & 0x1F) - 0x10;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (FieldFx_IsPlayerWithinRadius(&position, 0x19A)) {
            if (FX_TASK_AS(RoomShakeBurstEvent)->active) {
                channel = ANIM_OWNER_AS(RoomShakeBurstChannel);
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    (*((RoomShakeBurstEntity **)D_8009CD10))->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
        }
        if (spiral->timer < 0x18) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_GetActorPosition((char *)ANIM_OWNER_AS(RoomShakeBurstChannel)->pool, 0, (s16 *)&position);
        if (spiral->timer < 0x19) {
            glow = spiral->glow;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            g_FieldFxSpriteParams.depth = 0x28;
            FieldFx_SampleColorTrack(g_RoomM318EmberSpiralColorTrack, &color, (spiral->timer << 5) / 24);
            FieldFx_DrawStarFan(&position, 900, 800, 0xC, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            /* The angle shares its register with the model page below. */
            page = (spiral->timer << 10) / 24;
            depth = rsin(page) / 2 + 0x800;
            width = rsin(page) / 4 + 0xC00;
            FieldFx_AnglesToVector((GteShortVector *)spiral, 200, &tip);
            tip.x += position.x;
            tip.y += position.y;
            tip.z += position.z;
            rotation.x = spiral->x;
            rotation.y = spiral->y;
            rotation.z = spiral->z;
            rotation.y += 0x800;
            rotation.z += spiral->timer * 44;
            index = &g_FieldFxTpageSlot3;
            tpages = g_FieldFxTpageTable;
            {
                int tpage = tpages[*index];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.depth = 4;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            {
                int tpage = tpages[*index];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            {
                int kind;
                int palette;
                u16 rawPage = GetTPage(0, 1, 0, 0);
                page = (u16)(tpages[*index] | rawPage);
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                palette = GetClut(0x40, palette);
                FieldFx_ModelSetTexture(page, (u16)palette);
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = tip.x;
            matrix.t[1] = tip.y;
            matrix.t[2] = tip.z;
            scale.x = width;
            scale.y = width;
            scale.z = depth;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_RoomM318EmberSpiralModel);
            FieldFx_ModelScaleColors(g_RoomM318EmberSpiralModel, (u16)glow);
            FieldFx_ModelDraw(g_RoomM318EmberSpiralModel, &matrix);
            FieldFx_ModelRestoreColors(g_RoomM318EmberSpiralModel);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

int* RoomM318_SetActorParams(void* arg0, int arg1, int arg2) {
    int* base = g_RoomM318ActorParams;

    *base = arg2;
    g_RoomM318ActorParams[1] = arg1;

    return base;
}

int* RoomM318_SetEmberBurstParams(void* arg0, int arg1, int arg2) {
    int* base = g_RoomM318EmberBurstParams;

    *base = arg2;
    g_RoomM318EmberBurstParams[1] = arg1;

    return base;
}

int* RoomM318_SetRisingColumnParams(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        g_RoomM318RisingColumnParams[0] = arg1;
    } else {
        g_RoomM318RisingColumnParams[1] = arg1;
        g_RoomM318RisingColumnParams[2] = arg2;
        g_RoomM318RisingColumnParams[3] = arg3;
    }

    return g_RoomM318RisingColumnParams;
}
