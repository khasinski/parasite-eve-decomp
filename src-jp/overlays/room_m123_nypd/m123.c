#include "common.h"
#include "pe1/akao_send.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/room_floor.h"
#include "pe1/room_m123.h"
#include "pe1/room_m123_beacon.h"
#include "pe1/room_m123_glow_burst.h"
#include "pe1/room_m123_joint_triangle.h"
#include "pe1/room_sound_slot.h"

#include "m123_externs.h"
#include "room_m123_effects.h"

typedef struct RoomPulseObject RoomPulseObject;

typedef struct RoomPulsePool {
    RoomPulseObject *object;
} RoomPulsePool;

struct RoomPulseObject {
    u32 flags;
    u8 pad[0x14];
    u8 *status;
};

typedef struct RoomPulseGlobal {
    u8 pad[0x4C];
    u32 flags;
} RoomPulseGlobal;

typedef struct RoomM123Burst {
    u16 frame;
    u16 phase;
    u16 x, y, z;
    u16 pad;
} RoomM123Burst;

int RoomM123_JointTriangleCallback(int mode, RoomM123JointTriangle *triangle) {
    GteShortVector corners[4];
    GteShortVector offset = g_RoomM123JointOffset;
    GteShortVector tip = g_RoomM123JointOrigin;
    RenderColor color = g_RoomM123TriangleColor;
    int weight;
    int width;
    int position;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (triangle->state) {
        case 0:
            triangle->frame++;
            if (triangle->frame >= 24) return 1;
            break;
        case 1:
            triangle->frame++;
            triangle->position = (s16)(triangle->position + 0x200) % 0x3000;
            if (triangle->frame >= 24) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x28, &offset, &corners[0]);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x22, &offset, &corners[1]);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x1D, &tip, &corners[2]);
        switch (triangle->state) {
        case 0:
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x17, &tip, &corners[3]);
            weight = 0x5000 - rsin((triangle->frame << 10) / 24) * 4;
            LoadAverageShort12(&corners[3], &corners[0], 0x1000 - weight, weight, &corners[0]);
            LoadAverageShort12(&corners[3], &corners[1], 0x1000 - weight, weight, &corners[1]);
            LoadAverageShort12(&corners[3], &corners[2], 0x1000 - weight, weight, &corners[2]);
            width = (triangle->frame << 7) / 24;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            FieldFx_DrawGlowLine(&corners[0], &corners[1], &color, &color, width, width, 1);
            FieldFx_DrawGlowLine(&corners[1], &corners[2], &color, &color, width, width, 1);
            FieldFx_DrawGlowLine(&corners[2], &corners[0], &color, &color, width, width, 1);
            return 0;
        case 1:
            position = triangle->position;
            if (position < 0x1000) {
                weight = position;
                LoadAverageShort12(&corners[0], &corners[1], 0x1000 - weight, weight, &corners[0]);
            } else if (position < 0x2000) {
                weight = position - 0x1000;
                LoadAverageShort12(&corners[1], &corners[2], 0x1000 - weight, weight, &corners[0]);
            } else {
                weight = position - 0x2000;
                LoadAverageShort12(&corners[2], &corners[0], 0x1000 - weight, weight, &corners[0]);
            }
            matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            width = rsin(triangle->frame << 8) / 32;
            FieldFx_DrawHaloedPoint(&corners[0], &color, width, 1);
            break;
        default:
            return 0;
        }
        break;
    }
    return 0;
}

int RoomM123_JointTriangleEmitter(int mode) {
    int frame, index;
    RoomM123JointTriangleEmitterView *position;

    switch (mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 16,
                             (FieldFxCallbackListCallback)RoomM123_JointTriangleCallback);
    case 1:
        frame = g_FieldFxCurrentAge;
        if (frame == (frame / 6) * 6) {
            if (frame >= 40) return 2;
            for (index = 0; index < 3; index++) {
                position = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (position != 0) {
                    position->x = 1;
                    position->y = 0;
                    position->z = index << 12;
                }
            }
            if (g_FieldFxCurrentAge < 25) {
                position = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (position != 0) {
                    position->x = 0;
                    position->y = 0;
                    position->z = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 40) return 2;
        break;
    case 2: {
        int paletteIndex = g_FieldFxTpageSlot3;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[paletteIndex];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 12;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
    }
    return 0;
}

int RoomM123_GlowBurstParticleCallback(int mode, RoomM123PulsingParticle *particle) {
    GteShortVector position;
    GteShortVector output;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    int blend;
    u16 clut;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 16;
        if ((s16)particle->frame >= 48) return 1;
        break;
    case 2:
        scale = particle->scale *
                rcos(((s16)particle->frame << 10) / 48) / 4096;
        FieldFx_OffsetPointByAngles(&g_RoomM123GlowBurstCenter, &position, scale / 2, &g_RoomM123GlowBurstAngles,
                      scale, (s16)particle->offset);
        if ((s16)particle->frame < 16) {
            FieldFx_SampleColorTrack(g_RoomM123GlowColorTrack, &color, 0x30 - (scale * 48) / 1024);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads(&position, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xD8, clut, 3, g_RoomScriptRecord,
                          (RenderColor *)&color);
            matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            FieldFx_DrawHaloedPoint(&position, &color, g_RoomScriptRecord, 1);
            /* Retail re-tests the frame here; the blended tail only runs
             * from frame 16 on. */
            if ((s16)particle->frame < 16) break;
        }
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        FieldFx_SampleColorTrack(g_RoomM123GlowColorTrack, &color, 0x30 - (scale * 48) / 1024);
        blend = rcos(((s16)particle->frame - 16) << 5);
        LoadAverageShort12(&position, &g_RoomM123GlowBurstCenter, blend, 0x1000 - blend, &output);
        FieldFx_DrawGlowLine(&output, &position, &color, 0, g_RoomScriptRecord, 0, 1);
        break;
    }
    return 0;
}

/* Glow burst: spawns sixteen pulsing particles at frame 1, then draws four
 * glow layers, two flash rings and a swept flare on joint 23. The parameter
 * block fields written twice share a base register; the tile indices are
 * read as g_FieldFxTpageSlots elements so their loads stay below the base-register
 * stores, and the palette read-back follows the block's stores. */

int RoomM123_GlowBurst(int mode, RoomM123GlowBurst *burst) {
    GteShortVector template = g_RoomM123JointAnchor;
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    RoomM123BurstParticle *child;
    int i;
    int intensity;
    int step;
    int fade;

    switch (mode) {
    case 0:
        burst->timer = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 0x10,
                             (FieldFxCallbackListCallback)RoomM123_GlowBurstParticleCallback);
    case 1:
        if (burst->timer < 0x80) burst->timer += 4;
        if (g_FieldFxCurrentAge == 1) {
            for (i = 0; i < 16; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (child) {
                    child->frame = 0;
                    child->scale = (rand() & 0x1FF) + 0x200;
                    child->offset = rand();
                }
            }
            FieldFx_PlaySoundAtOwner(0x580, FieldFx_GetOwnerProgress());
            FieldFx_PlaySoundAtOwner(0x5BD, 0x80);
        }
        if (g_FieldFxCurrentAge >= 33) return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x40;
        g_FieldFxSpriteParams.parameter02 = 4;
        g_FieldFxSpriteParams.extent_x = 0x40;
        g_FieldFxSpriteParams.extent_y = 0x40;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0x32;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x17, &template, &position);
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -g_FieldFxCurrentAge << 6;
        rotation.flags = 0;
        intensity = burst->timer;
        if (g_FieldFxCurrentAge != 0) intensity = intensity * 2 / 3;
        rcos((g_FieldFxCurrentAge << 10) / 40);
        {
            int kind;
            int palette;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x50,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 6 : palette + 2),
                          1, intensity, 0, 0);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x4D, GetClut(0x50, palette),
                          1, intensity, 0, 0xA00);
        }
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x48, GetClut(0x30, palette),
                          1, intensity, 0, 0xC00);
        }
        g_FieldFxSpriteParams.parameter00 = 0x40;
        g_FieldFxSpriteParams.parameter02 = 4;
        g_FieldFxSpriteParams.extent_x = 0x40;
        g_FieldFxSpriteParams.extent_y = 0x40;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x49, GetClut(0x40, palette),
                          1, intensity, 0, 0xE00);
        }
        FieldFx_SampleColorTrack(g_RoomM123GlowColorTrack, &color, g_FieldFxCurrentAge / 2);
        FieldFx_DrawStarFan(&position, 700, 700, 0x10, 0, 0x1000, 0x1000, &color, 0, 0x40, 1);
        FieldFx_SampleColorTrack(g_RoomM123GlowBurstColorTrack, &color, g_FieldFxCurrentAge * 48 / 40);
        FieldFx_DrawStarFan(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        g_RoomScriptRecord = burst->timer;
        g_RoomM123GlowBurstCenter.x = position.x;
        g_RoomM123GlowBurstCenter.y = position.y;
        g_RoomM123GlowBurstCenter.z = position.z;
        FieldFx_GetTransformLookAngles(ANIM_OWNER_AS(RoomM123Pool)->pool, 0, &g_RoomM123GlowBurstAngles.vector);
        step = g_FieldFxCurrentAge - 16;
        if ((unsigned int)step < 17) {
            fade = rcos(step << 6);
            FieldFx_SampleColorTrack(g_RoomM123GlowBurstColorTrack, &color, step * 3);
            g_RoomM123GlowBurstAngles.rotation.flags = 1;
            FieldFx_DrawRingBand(&position, 700, 1000, 0x18, &g_RoomM123GlowBurstAngles.rotation, fade, fade, 0, &color,
                          0x80, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot2];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.depth = 0xC;
        break;
    }
    return 0;
}

/* Joint beacon: rides joint 0x17, swells in, pulses and fades out, tagging
 * the player once on contact; draws a glow sprite pair with two star fans,
 * a fading three-sprite ring on the ground, a turning model while the effect
 * starts and three scaled passes of a second model. */
int RoomM123_JointBeacon(int mode, RoomM123Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM123BeaconOffset offset = g_RoomM123BeaconAnchor;
    GteMatrix matrix;
    GteMatrix spin;
    RoomM123BeaconColor color;
    int size;
    int page;

    switch (mode) {
    case 0:
        state->fade = 200;
        state->phase = 0;
        state->timer = 0;
        state->glow = 0;
        state->width = 0;
        state->depth = 0x1000;
        if (FX_TASK_AS(RoomM123BeaconEvent)->active) {
            RoomM123BeaconPool *pool = ANIM_OWNER_AS(RoomM123BeaconChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->armed = 1;
        g_RoomM123BeaconModel0 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5483704);
        g_RoomM123BeaconModel1 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5883704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM123BeaconModel0, 0, 0);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM123BeaconModel1, 0, 0xF0);
        return 0;
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123BeaconChannel)->pool, 0x17, &offset, state);
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM123BeaconChannel)->pool, 0,
                      &state->rotation);
        state->alpha = 0x80;
        switch (state->phase) {
        case 0:
            state->timer++;
            state->height = rcos(state->timer << 7) / 2 + 0x1000;
            if (state->width < 0x800) state->width += 0x100;
            state->depth -= 0x80;
            state->glow = rcos(state->timer << 7) + 0x1000;
            state->shade = state->timer;
            if (state->timer >= 8) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->height = 0x1000;
            state->glow = ((g_FieldFxCurrentAge & 1) << 9) + 0x1000;
            if (state->shade < 0x20) state->shade++;
            if (state->timer >= 0x20) {
                state->phase = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->height = 0x1000;
            state->width = 0x800;
            state->glow = 0x1000;
            state->timer++;
            state->shade++;
            state->alpha = 0x80 - state->timer * 8;
            if (state->timer >= 0x10) return 1;
            break;
        }
        FieldFx_AnglesToVector(&state->rotation, 0x7D0, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (FieldFx_IsPlayerInSegmentBand(state, &state->target, 0x140) == 0) return 0;
        if (state->armed == 0) return 0;
        if (FX_TASK_AS(RoomM123BeaconEvent)->active) {
            RoomM123BeaconChannel *channel = ANIM_OWNER_AS(RoomM123BeaconChannel);
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                ((RoomM123BeaconEntity *)D_8009CD10)->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        state->armed = 0;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 0x32;
        g_FieldFxSpriteParams.parameter00 = 0x40;
        g_FieldFxSpriteParams.parameter02 = 4;
        g_FieldFxSpriteParams.extent_x = 0x40;
        g_FieldFxSpriteParams.extent_y = 0x40;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        if (state->glow != 0) {
            rotation.x = 0;
            rotation.y = 0;
            rotation.pad = 0;
            rotation.z = g_FieldFxCurrentAge << 6;
            size = state->glow;
            FieldFx_SampleColorTrack(g_RoomM123BeaconColorTrack, &color, state->shade);
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&state->position, (GteRotation *)&rotation, size * 3 / 2, size * 3 / 2, 0x50,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 6
                                                                  : palette + 2),
                              1, 0xA0, (struct RenderColor *)&color);
            }
            FieldFx_DrawStarFan(&state->position, 0x2BC, 0x2BC, 0xC, 0, size, size, (struct RenderColor *)&color, 0, 0x50, 1);
            FieldFx_DrawStarFan(&state->position, 0xF0, 0xF0, 8, 0, size, size, (struct RenderColor *)&color, 0, 0x80, 1);
            if (state->fade > 0) {
                state->fade -= 8;
                if (state->fade > 0) {
                    position.x = state->position.x;
                    position.y = state->position.y;
                    position.z = state->position.z;
                    g_FieldFxSpriteParams.parameter00 = 0x20;
                    g_FieldFxSpriteParams.parameter02 = 2;
                    g_FieldFxSpriteParams.extent_x = 0x20;
                    g_FieldFxSpriteParams.extent_y = 0x20;
                    g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
                    g_FieldFxSpriteParams.palette = 3;
                    g_FieldFxSpriteParams.parameter06 = 0;
                    {
                        int kind = g_FieldFxSpriteParams.palette;
                        int palette = g_FieldFxPaletteTable[kind];
                        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                        FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x4D, GetClut(0x50, palette), 1,
                                      state->fade, 0, 0xA00);
                    }
                    g_FieldFxSpriteParams.parameter00 = 0x10;
                    g_FieldFxSpriteParams.parameter02 = 1;
                    g_FieldFxSpriteParams.extent_x = 0x10;
                    g_FieldFxSpriteParams.extent_y = 0x10;
                    {
                        int kind = g_FieldFxSpriteParams.palette;
                        int palette = g_FieldFxPaletteTable[kind];
                        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                        FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x48, GetClut(0x30, palette), 1,
                                      state->fade, 0, 0xC00);
                    }
                    g_FieldFxSpriteParams.parameter00 = 0x40;
                    g_FieldFxSpriteParams.parameter02 = 4;
                    g_FieldFxSpriteParams.extent_x = 0x40;
                    g_FieldFxSpriteParams.extent_y = 0x40;
                    {
                        int kind = g_FieldFxSpriteParams.palette;
                        int palette = g_FieldFxPaletteTable[kind];
                        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                        FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, 0x49, GetClut(0x40, palette), 1,
                                      state->fade, 0, 0xE00);
                    }
                }
            }
        }
        {
            u16 *index = &g_FieldFxTpageSlot3;
            u16 *tpages = g_FieldFxTpageTable;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            if (g_FieldFxCurrentAge < 0x11) {
                RenderMatrixSlot *slot;
                rotation.x = 0x800;
                rotation.y = 0;
                rotation.z = g_FieldFxCurrentAge << 4;
                RotMatrixYXZ(&rotation, &spin);
                matrix = *ANIM_OWNER_AS(RoomM123BeaconChannel)->pool->matrix;
                MulMatrix0(&matrix, &spin, &matrix);
                scale.x = scale.y = rsin(g_FieldFxCurrentAge << 6) / 4 + 0x400;
                scale.z = 0x1000;
                ScaleMatrix(&matrix, &scale);
                matrix.t[0] = state->position.x;
                matrix.t[1] = state->position.y;
                matrix.t[2] = state->position.z;
                slot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
                g_FieldFxSpriteParams.tpage = tpages[*index];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
                {
                    int kind = g_FieldFxSpriteParams.palette;
                    int palette = g_FieldFxPaletteTable[kind];
                    if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                    FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
                }
                FieldFx_ModelSetSemiTrans(1);
                FieldFx_ModelSaveColors(g_RoomM123BeaconModel1);
                FieldFx_ModelSetColors(g_RoomM123BeaconModel1, color.r, color.g, color.b);
                size = rcos(g_FieldFxCurrentAge << 6) / 64;
                FieldFx_ModelScaleColors(g_RoomM123BeaconModel1, size);
                FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM123BeaconModel1, &matrix);
                FieldFx_ModelRestoreColors(g_RoomM123BeaconModel1);
            }
        }
        rotation.x = 0x800;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 7;
        RotMatrixYXZ(&rotation, &spin);
        matrix = *ANIM_OWNER_AS(RoomM123BeaconChannel)->pool->matrix;
        MulMatrix0(&matrix, &spin, &matrix);
        scale.x = scale.y = state->height * 2 / 5;
        scale.z = 0x1800;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RenderMatrixSlot *slot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
        }
        {
            u16 *index = &g_FieldFxTpageSlot3;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(g_RoomM123BeaconModel0);
        FieldFx_ModelSetColors(g_RoomM123BeaconModel0, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM123BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM123BeaconModel0);
        FieldFx_ModelSaveColors(g_RoomM123BeaconModel0);
        scale.z = 0x1000;
        scale.x = scale.y = state->depth;
        ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &g_FieldFxTpageSlot3;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSetColors(g_RoomM123BeaconModel0, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM123BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM123BeaconModel0);
        FieldFx_ModelSaveColors(g_RoomM123BeaconModel0);
        scale.z = 0x1000;
        scale.x = scale.y = state->width;
        ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &g_FieldFxTpageSlot3;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSetColors(g_RoomM123BeaconModel0, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM123BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM123BeaconModel0);
        break;
    }
    return 0;
}

int RoomM123_JointSwirlParticleCallback(int mode, RoomM123Particle *particle) {
    GteShortVector position;
    int textureFrame, scale, kind, palette;
    u16 clut;

    switch (mode) {
    case 1: {
        int frame = particle->frame;
        int speed = g_RoomM123SwirlSpeed;
        int shift = speed * 24;

        frame++;
        particle->frame = frame;
        particle->offset += shift / 4096;
        if ((s16)particle->frame >= 24) return 1;
    } break;
    case 2: {
        int sampleFrame;

        scale = g_RoomM123SwirlSpeed * particle->scale;
        sampleFrame = (s16)particle->frame;
        FieldFx_OffsetPointByAngles(g_RoomM123SwirlOrigin, &position, sampleFrame << 6, g_RoomM123SwirlAngles,
                      scale / 4096, (s16)particle->offset);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(0x20, palette);
        textureFrame = (s16)g_FieldFxSpriteParams.parameter02 * (particle->frame & 7) + 0x40;
        FieldFx_DrawShapeQuads(&position, 0, 0x555, 0x555, textureFrame, clut, 1,
                      g_RoomM123SwirlBrightness, 0);
    } break;
    }
    return 0;
}

int RoomM123_JointSwirl(int mode)
{
    GteShortVector seed = g_RoomM123JointAnchor;
    GteShortVector output;
    RoomM123Particle *particle;
    int i;
    int time;
    int scale;
    u16 outX, outY, outZ;
    RoomM123Pool *context;
    void *texture;
    u16 palette;

    switch (mode) {
    case 0:
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 24,
                             (FieldFxCallbackListCallback)RoomM123_JointSwirlParticleCallback);
    case 1:
        if (g_FieldFxCurrentAge == 1) {
            for (i = 0; i < 24; i++) {
                particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (particle != 0) {
                    particle->frame = (i << 6) / 24;
                    particle->scale = (rand() & 63) + 64;
                    particle->offset = rand();
                }
            }
        } else {
            particle = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (particle != 0) {
                particle->frame = 0;
                particle->scale = (rand() & 63) + 64;
                particle->offset = rand();
            }
        }
        time = g_FieldFxCurrentAge;
        g_RoomM123SwirlSpeed = 0x1000;
        if (time >= 50)
            g_RoomM123SwirlSpeed = 0x1000 - ((time - 50) << 12) / 30;
        if (time < 80) break;
        return 1;
    case 2:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 23, &seed, &output);
        scale = rsin((g_FieldFxCurrentAge << 11) / 80);
        if (scale < 0)
            scale += 31;
        outX = output.x;
        outY = output.y;
        outZ = output.z;
        context = ANIM_OWNER_AS(RoomM123Pool);
        /* Keep the context load ahead of the texture address. */
        asm volatile("" : : "r"(context));
        texture = g_RoomM123SwirlAngles;
        g_RoomM123SwirlOrigin[0] = outX;
        g_RoomM123SwirlOrigin[1] = outY;
        g_RoomM123SwirlOrigin[2] = outZ;
        /* Finish the position writes before reducing the signed scale. */
        asm volatile("" : : : "memory");
        g_RoomM123SwirlBrightness = scale >> 5;
        FieldFx_GetTransformLookAngles(context->pool, 0, texture);
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 12;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
ret0:
    return 0;
}

int RoomM123_PulseRingCallback(int mode, RoomPulseParticle *particle, int *reference)
{
    int color[2];
    GteShortVector position = g_RoomM123PulseRingTilt;
    GteShortVector *positionPtr;
    GteMatrix **matrixSlot;
    int wave;

    positionPtr = &position;
    switch (mode) {
    case 1:
        if (particle->state != 0)
            break;
        particle->frame++;
        if (particle->frame >= 24)
            return 1;
        break;
    case 2:
        if (particle->state != 0)
            break;
        matrixSlot = &g_ViewMatrixSlot.value;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        FieldFx_SampleColorTrack((void *)*reference, color, particle->frame);
        wave = rsin((g_FieldFxCurrentAge << 10) / 24);
        FieldFx_DrawRingBand((GteShortVector *)particle, 1000, 1500, 24, (GteRotation *)positionPtr, wave, wave, 0, (struct RenderColor *)color, 128, 1);
        break;
    }
    return 0;
}

int RoomM123_PulseRing(int mode, RoomPulseParticle *particle)
{
    RoomPulseParticle *next;
    RoomPulsePool *pool;
    RoomPulseObject *object;
    RoomM123Pool *context;
    RoomSoundSlot *soundSlot;
    int time;
    int volume;
    u16 palette;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition(ANIM_OWNER_AS(RoomM123Pool)->pool, 1, &particle->z);
        particle->x = 0;
        particle->y = 0;
        if (FX_TASK_AS(u8)[13]) {
            pool = ANIM_OWNER_AS(RoomM123Pool)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            time = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(soundSlot->channel, 0x582, time, 0x80, volume);
            if (soundSlot->channel != 0)
                Akao_SendTableCommand(soundSlot->channel, 0x5B1, 0x80, 0x80, volume);
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM123Pool)->pool, 12, 8,
                             (FieldFxCallbackListCallback)RoomM123_PulseRingCallback);
    case 1:
        if (g_FieldFxCurrentAge < 32 && g_FieldFxCurrentAge % 6 == 0) {
            next = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (next != 0) {
                next->x = particle->z;
                next->y = particle->pad;
                next->z = particle->state;
                next->y = g_RoomFloorY->raw;
                next->state = 0;
                next->frame = 0;
            }
        }
        if (g_FieldFxCurrentAge == 32 && FX_TASK_AS(u8)[13]) {
            context = ANIM_OWNER_AS(RoomM123Pool);
            if ((((RoomPulsePool *)context->pool)->object->flags & 0x3F000000) == 0x01000000) {
                (*((RoomPulseGlobal **)D_8009CD10))->flags |= 0x4000;
                ((RoomPulsePool *)context->pool)->object->flags =
                    (((RoomPulsePool *)context->pool)->object->flags & 0xC0FFFFFF) | 0x21000000;
                ((RoomPulsePool *)context->pool)->object->flags |= 0x80000000;
            }
        }
        if (g_FieldFxCurrentAge < 8) break;
        return 2;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        palette = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
ret0:
    return 0;
}

int RoomM123_WaveCallback(int mode, RoomM123Wave *wave) {
    int color[2];
    GteShortVector position = g_RoomM123WaveSpinBase;
    int result, scale, kind, palette;
    u16 clut;
    switch (mode) {
    case 1:
        if (wave->state != 0) break;
        wave->frame++;
        wave->y -= 3;
        if ((s16)wave->frame >= 24) return 1;
        break;
    case 2:
        if (wave->state != 0) break;
        FieldFx_SampleColorTrack(g_RoomM123WaveColorTrack, color, (s16)wave->frame);
        result = rcos((g_FieldFxCurrentAge << 10) / 24);
        position.z = g_FieldFxCurrentAge << 5;
        scale = result / 2 + 2048;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(48, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)wave, (GteRotation *)&position, scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 + 220, clut, 1, 128, (struct RenderColor *)color);
        break;
    }
    return 0;
}

int RoomM123_WaveBurst(int mode, RoomM123Burst *burst, int *choice) {
    GteShortVector position = g_RoomM123JointOffset;
    RoomM123Wave *child;
    int value;

    switch (mode) {
    case 0:
        value = *choice ? 34 : 40;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, value, &position, &burst->x);
        burst->frame = 0;
        burst->phase = 0;
        FieldFx_PlaySoundAtOwner(0x581, FieldFx_GetOwnerProgress());
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM123Pool)->pool, 12, 24,
                             (FieldFxCallbackListCallback)RoomM123_WaveCallback);
    case 1:
        if (g_FieldFxCurrentAge < 32) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (child != 0) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                {
                    int sample = rand();
                    int previous = child->x;
                    child->x = (previous - 128) + (sample & 255);
                }
                {
                    int sample = rand();
                    int previous = child->y;
                    child->y = (previous - 128) + (sample & 255);
                }
                {
                    int sample = rand();
                    int previous = child->z;
                    child->z = (previous - 128) + (sample & 255);
                }
                child->state = 0;
                child->frame = 0;
            }
        }
        if (g_FieldFxCurrentAge >= 8) return 2;
        break;
    case 2: {
        int paletteIndex = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[paletteIndex];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
    }
    return 0;
}
