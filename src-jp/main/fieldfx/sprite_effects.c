#include "pe1/battle_runtime.h"
#include "pe1/field_actor.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/fieldfx_task.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/room_ember_burst.h"

/* One falling sprite: drifts with damped sideways speed, bounces off the
 * floor and spins through a seven-frame strip tinted by the scene track. */
int FieldFx_FallingSprite(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    u8 *track;
    int scale;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 6 / 7;
        state->velocity_z = state->velocity_z * 6 / 7;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (g_FieldFxCurrentAge >= state->duration)
            return 1;
        break;
    case 2:
        switch (g_FieldFxTaskContext->variables[6]) {
        case 0:
            track = g_FallingSpriteColorTrack0;
            break;
        case 1:
            track = g_FallingSpriteColorTrack1;
            break;
        default:
            track = g_BouncingSpriteColorTrack2;
            break;
        }
        FieldFx_SampleColorTrack(track, &color, g_FieldFxCurrentAge * 48 / state->duration);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (g_FieldFxCurrentAge << 7);
        rotation.flags = 0;
        scale = (g_FieldFxCurrentAge << 11) / state->duration + 0x400;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        clut = GetClut(32, palette);
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge % 7) + 0xE8, clut, 1, 128, &color);
        break;
    }
    return 0;
}

/* Drops one falling sprite per frame for eight frames from above the
 * player (lower in scene variant 0) on an angle that turns by 0x200. */
int FieldFx_FallingSpriteEmitter(int mode, RenderPhaseEmitter *state)
{
    GteShortVector position;
    RenderBouncingSprite *sprite;
    int speed;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 16,
                             (FieldFxTaskCallback)FieldFx_FallingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 8) {
            for (i = 0; i < 1; i++) {
                sprite = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (sprite) {
                    FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
                    sprite->x = position.x;
                    if (g_FieldFxTaskContext->variables[6] == 0)
                        sprite->y = position.y - 600;
                    else
                        sprite->y = position.y - 400;
                    sprite->z = position.z;
                    speed = (rand() & 7) + 32;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 26;
                    sprite->angle = rand();
                    state->phase += 0x200;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 40)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.extent_x *= 2;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 32;
        break;
    }
    return 0;
}

int FieldFx_PulsingStarFan(int mode)
{
    GteShortVector position;
    RenderColor color;
    u8 *track;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (g_FieldFxCurrentAge >= 20)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 200;
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
        position.y -= 300;
        track = g_FieldFxTaskContext->variables[6] == 1 ? g_PulsingStarFanColorTrack1 : g_PulsingStarFanColorTrack0;
        FieldFx_SampleColorTrack(track, &color, g_FieldFxCurrentAge * 24 / 20);
        scale = rsin((g_FieldFxCurrentAge << 10) / 20) / 2 + 2048;
        FieldFx_DrawStarFan(&position, 400, 400, 8, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_ScatterBurstSprite(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, scale, palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 31 / 32;
        state->velocity_z = state->velocity_z * 31 / 32;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (g_FieldFxCurrentAge >= state->duration)
            return 1;
        break;
    case 2:
        intensity = 128 - (g_FieldFxCurrentAge << 7) / state->duration;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (g_FieldFxCurrentAge << 3);
        scale = rsin((g_FieldFxCurrentAge << 10) / state->duration) + 4096;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        clut = GetClut(64, palette);
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale,
            (s16)g_FieldFxSpriteParams.parameter02 * ((g_FieldFxCurrentAge << 3) / state->duration) + 128,
            clut, 1, intensity, 0);
        break;
    }
    return 0;
}


/* Scatter burst at the actor target: opens a pool of bouncing sprites
 * (FieldFx_ScatterBurstSprite), throws two per frame for six frames on a turning angle,
 * and draws a growing flash, a short ring and a ground ring. */
int FieldFx_ScatterBurstEmitter(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = g_FieldFxGlowFountainTint;
    RenderBouncingSprite *sprite;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 12,
                             (FieldFxTaskCallback)FieldFx_ScatterBurstSprite);
    case 1:
        if (g_FieldFxCurrentAge < 6) {
            for (i = 0; i < 2; i++) {
                sprite = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (sprite) {
                    sprite->x = state->position.x;
                    sprite->y = state->position.y;
                    sprite->z = state->position.z;
                    speed = (rand() & 15) + 43;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 17;
                    sprite->angle = rand();
                    state->phase += 0x955;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[8]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 100;
        state->position.y -= 6;
        intensity = rcos((g_FieldFxCurrentAge << 10) / 24) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 4;
        rotation.flags = 0;
        scale = 0x3000 - rcos((g_FieldFxCurrentAge << 10) / 12);
        i = (g_FieldFxCurrentAge << 3) / 12;
        if (i > 7)
            i = 7;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * i,
                      GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 5 : palette + 1),
                      1, intensity, 0);
        if (g_FieldFxCurrentAge < 9) {
            intensity = rcos(g_FieldFxCurrentAge << 7) / 32;
            FieldFx_DrawStarFan(&state->position, 250, 250, 12, 0, scale, scale,
                          &color, 0, intensity, 1);
        }
        g_FieldFxSpriteParams.depth = 4;
        if ((unsigned)(g_FieldFxCurrentAge - 4) < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 7;
            rotation.flags = 1;
            intensity = 128 - ((g_FieldFxCurrentAge - 4) << 7) / 12;
            scale = rsin(((g_FieldFxCurrentAge - 4) << 10) / 12) * 2;
            position.y = g_RoomFloorY->y;
            FieldFx_DrawRingBand(&position, 250, 400, 12, &rotation, scale, scale, 0,
                          &color, intensity, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 64;
        break;
    }
    return 0;
}

int FieldFx_CosineEffectB(int mode, RenderCosineEffect *state)
{
    RenderColor color = g_FieldFxCosineEffectTint;

    switch (mode) {
    case 1:
        state->x = state->amplitude
            * rcos((g_FieldFxCurrentAge << 10) / state->duration) / 4096;
        if (g_FieldFxCurrentAge >= state->duration)
            return 1;
        break;
    case 2:
        FieldFx_DrawOrbitPoint(state->x, state->y, &color,
            rcos((g_FieldFxCurrentAge << 10) / state->duration) / 64 + 64, 1);
        break;
    }
    return 0;
}


/* Arcing emitter: spawns three cosine sparks per frame between frames 4
 * and 10, draws the expanding ring for 13 frames and anchors the effect at
 * its position under the current view matrix. */
int FieldFx_CosineSparkEmitter(int mode, RenderArcingEmitter *state)
{
    GteShortVector position;
    RenderColor color = g_FieldFxArcingEmitterTint;
    RenderCosineEffect *effect;
    int scale;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 24,
                             (FieldFxTaskCallback)FieldFx_CosineEffectB);
    case 1:
        if (g_FieldFxCurrentAge >= 4 && g_FieldFxCurrentAge < 11) {
            for (i = 0; i < 3; i++) {
                effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (effect) {
                    effect->amplitude = state->radius;
                    effect->duration = (rand() & 3) + 12;
                    effect->y = state->phase;
                    state->phase += 0x8AA + (rand() & 31);
                }
            }
        }
        if (g_FieldFxCurrentAge >= 32)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 64;
        if (g_FieldFxCurrentAge < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((g_FieldFxCurrentAge << 10) / 12);
            state->radius = scale / 8;
            FieldFx_SampleColorTrack(g_CosineSparkColorTrack, &color, g_FieldFxCurrentAge);
            FieldFx_DrawRingBand(&position, 300, 512, 12, 0, scale, scale, 0,
                          &color, 128, 1);
        }
        g_FieldFxSpriteParams.depth = 64;
        gte_ldrotmatrix((const GteMatrixWords *)g_ViewMatrixSlot.value);
        gte_ldtransmatrix((const GteMatrixWords *)g_ViewMatrixSlot.value);
        FieldFx_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}

static inline int TextureIndex(unsigned int factor, unsigned int step)
{
    return factor * step + 128;
}

int FieldFx_RisingSprite(int mode, RenderRisingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    u16 clut;
    int y;
    int random;
    u16 velocity;
    unsigned int frame;
    int phase;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        random = rand();
        velocity = state->vy + 2;
        state->vy = velocity + (random & 1);
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        FieldFx_SampleColorTrack(g_RisingSpriteColorTrack, &color, g_FieldFxCurrentAge);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = g_FieldFxCurrentAge;
        rotation.z = state->angle + (frame << 5);
        phase = frame << 10;
        scale = rsin(phase / 24) + 0x1000;
        y = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            y += 4;
        clut = GetClut(32, y);
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale,
            TextureIndex((s16)g_FieldFxSpriteParams.parameter02, g_FieldFxCurrentAge / 3),
            clut, 1, 128, &color);
        break;
    }
    return 0;
}

/* Rising effect emitter at the actor target: plays sound 0x487, opens a pool
 * of rising sprites (FieldFx_RisingSprite), scatters one per frame for eight frames
 * and draws a slowly turning glow while the anchor climbs. */
int FieldFx_RisingSpriteEmitter(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    RenderRisingEffect *effect;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    u16 clut;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        Asset_Find08Alt(0x487, 0, state->position.x, state->position.y,
                        state->position.z);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 8,
                             (FieldFxTaskCallback)FieldFx_RisingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 8) {
            effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (effect) {
                effect->x = state->position.x + (rand() & 0x7F) - 64;
                effect->y = state->position.y + (rand() & 0xFF) - 128;
                effect->z = state->position.z + (rand() & 0x7F) - 64;
                speed = (rand() & 7) + 16;
                effect->vx = rsin(state->phase) * speed / 4096;
                effect->vz = rcos(state->phase) * speed / 4096;
                effect->vy = -(rand() & 7) - 12;
                effect->angle = rand();
                state->phase += 0x955;
            }
        }
        if (g_FieldFxCurrentAge >= 30)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 64;
        state->position.y -= 4;
        intensity = rcos((g_FieldFxCurrentAge << 10) / 30) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -0x200;
        rotation.flags = 0;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        scale = 0x2400;
        if (kind == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        clut = GetClut(32, palette);
        FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale, 0x6C, clut,
                      1, intensity, 0);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 64;
        break;
    }
    return 0;
}

/* One sprite of the spiral: circles the shared anchor g_SpiralEffectAnchor at radius
 * g_SpiralEffectRadius while sinking, and flickers through an eight-frame strip. */
int FieldFx_SpiralSprite(int mode, RenderSpiralSprite *state)
{
    RenderColor color = g_FieldFxSpiralSpriteTint;
    int scale;
    int intensity;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->position.y = state->height;
        state->position.x = g_SpiralEffectAnchor.x + g_SpiralEffectRadius * rcos(state->angle) / 4096;
        state->position.z = g_SpiralEffectAnchor.z + g_SpiralEffectRadius * rsin(state->angle) / 4096;
        state->height -= 7;
        state->angle += 2;
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        scale = (g_FieldFxCurrentAge << 11) / 24 + 0x800;
        intensity = 128 - (g_FieldFxCurrentAge << 7) / 24;
        state->position.y = g_SpiralEffectAnchor.y - (rand() & 31);
        scale = scale * (((state->phase + g_FieldFxCurrentAge / 2) & 7) * 128 + 0x1000) / 4096;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        clut = GetClut(64, palette);
        FieldFx_DrawShapeQuads(&state->position, 0, scale * 2, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * ((state->phase + g_FieldFxCurrentAge / 2) & 7) + 128,
                      clut, 1, intensity, &color);
        break;
    }
    return 0;
}

/* Spiral emitter on the ground under the actor: releases 24 circling
 * sprites (FieldFx_SpiralSprite) on the first frame, publishes the anchor and the
 * swelling radius they follow, and draws a spinning model in two sizes. */
int FieldFx_SpiralEmitter(int mode, RenderArcingEmitter *state)
{
    GteMatrix matrix;
    GteVector scale;
    GteShortVector angles;
    RenderSpiralSprite *sprite;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;
    int angle;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        state->position.y = g_RoomFloorY->y;
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_FieldFxModel6DEAE684, 0, 0xC0);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 24,
                             (FieldEffectCallback)FieldFx_SpiralSprite);
    case 1:
        if (g_FieldFxCurrentAge == 0) {
            for (i = 0, angle = 0; i < 24; i++, angle += 0xAA) {
                sprite = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (sprite) {
                    sprite->height = state->position.y;
                    sprite->phase = rand();
                    sprite->angle = angle;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        g_SpiralEffectRadius = rsin((g_FieldFxCurrentAge << 10) / 24) / 3;
        g_SpiralEffectAnchor.x = state->position.x;
        g_SpiralEffectAnchor.y = state->position.y;
        g_SpiralEffectAnchor.z = state->position.z;
        break;
    case 2:
        size = rsin((g_FieldFxCurrentAge << 10) / 24) / 2;
        intensity = rcos((g_FieldFxCurrentAge << 10) / 24) / 32;
        scale.x = scale.y = scale.z = size;
        angles.x = 0;
        angles.y = g_FieldFxCurrentAge << 5;
        angles.z = 0;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        slots = g_FieldFxTpageSlots;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[slots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        tpages = g_FieldFxTpageTable;
        g_FieldFxSpriteParams.parameter06 = 0;
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        {
            int tpage = (u16)(tpages[slots[0]] | GetTPage(0, 1, 0, 0));
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag)
                palette += 4;
            FieldFx_ModelSetTexture(tpage, GetClut(0xC0, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(g_FieldFxModel6DEAE684);
        FieldFx_ModelScaleColors(g_FieldFxModel6DEAE684, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_FieldFxModel6DEAE684, &matrix);
        scale.x /= 2;
        scale.y /= 2;
        scale.z /= 2;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        FieldFx_ModelDraw((union FieldTexturedModel *)g_FieldFxModel6DEAE684, &matrix);
        FieldFx_ModelRestoreColors(g_FieldFxModel6DEAE684);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 64;
        break;
    }
    return 0;
}

/* Ring pulse at the actor target: a breathing disc for 32 frames and an
 * expanding ground ring over the first 17. */
int FieldFx_RingPulse(int mode, RenderRingPulse *state)
{
    GteRotation rotation;
    RenderColor color = g_FieldFxRingPulseTint;
    int scale;
    int intensity;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        state->raised.x = state->position.x;
        state->raised.y = state->position.y;
        state->raised.z = state->position.z;
        state->raised.y -= 0x1B8;
        return 0;
    case 1:
        if (g_FieldFxCurrentAge >= 32)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 100;
        scale = rcos(g_FieldFxCurrentAge << 5) / 2 + 0x800;
        intensity = rcos(g_FieldFxCurrentAge << 5) / 32;
        FieldFx_DrawStarFan(&state->position, 700, 700, 12, 0, scale, scale,
                      &color, 0, intensity, 1);
        g_FieldFxSpriteParams.depth = 0;
        if (g_FieldFxCurrentAge < 17) {
            intensity = 128 - (g_FieldFxCurrentAge << 3);
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 7;
            rotation.flags = 1;
            scale = rsin(g_FieldFxCurrentAge << 6) * 2;
            FieldFx_DrawRingBand(&state->position, 800, 1000, 16, &rotation, scale,
                          scale, 0, &color, intensity, 1);
        }
        break;
    }
    return 0;
}

int FieldFx_BouncingSpinSprite(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = g_FieldFxBouncingSpriteTint;
    int scale, intensity, palette;
    unsigned int frame, angle;
    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 31 / 32;
        state->velocity_z = state->velocity_z * 31 / 32;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (g_FieldFxCurrentAge >= state->duration)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = g_FieldFxCurrentAge;
        intensity = 128;
        angle = state->angle;
        rotation.z = (angle << 8) + (frame << 7);
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        scale = (state->angle & 0x7FF) + 0x400;
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 0xBC,
            GetClut(128, palette), 255, intensity, &color);
        break;
    }
    return 0;
}

int FieldFx_BouncingSpinEmitter(int mode, RenderPhaseEmitter *state) {
    RenderBouncingSprite *particle;
    GteShortVector position;
    int count;
    int amplitude;

    switch (mode) {
    case 0:
        state->phase = rand();
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 24,
                             (FieldFxTaskCallback)FieldFx_BouncingSpinSprite);
    case 1:
        if (g_FieldFxCurrentAge < 8) {
            for (count = 0; count < 3; count++) {
                particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (particle != 0) {
                    position.x = g_FieldFxOwner->actor->render_object.target_x;
                    position.y = g_FieldFxOwner->actor->render_object.target_y;
                    position.z = g_FieldFxOwner->actor->render_object.target_z;
                    particle->x = position.x;
                    particle->y = position.y;
                    particle->z = position.z;
                    state->phase += 0x955 + (rand() & 31);
                    amplitude = (rand() & 127) + 70;
                    particle->velocity_x = rsin(state->phase) * amplitude / 4096;
                    particle->velocity_z = rcos(state->phase) * amplitude / 4096;
                    particle->velocity_y = -(rand() & 31) - 36;
                    particle->duration = (rand() & 7) + 26;
                    particle->angle = rand();
                }
            }
        }
        if (g_FieldFxCurrentAge >= 40)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[0]];
        g_FieldFxSpriteParams.palette = 0;
        FieldFx_EnsureEffectTexturePage(0);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0;
        break;
    }
    return 0;
}

/* Rising ember sparks: the drifting ember callback and the fan-sweep
 * spark template instance. */

int FieldFx_EmberSpark(int mode, s16 *state) {
    s16 sp10[3];
    char sp18[8];

    switch (mode) {
    case 1:
        state[0] += (rand() & 7) - 3;
        state[1] += (rand() & 3) - 7;
        state[2] += (rand() & 7) - 3;

        if (g_FieldFxCurrentAge < 0x12) {
            break;
        }
        return 1;
    case 2:
        sp10[0] = state[0];
        sp10[1] = state[1];
        sp10[2] = state[2];
        FieldFx_SampleColorTrack(g_EmberColorTrack, sp18, g_FieldFxCurrentAge);
        FieldFx_DrawHaloedPoint(sp10, sp18, 0x80, 1);
        break;
    }

    return 0;
}

#define ROOM_FAN_SWEEP_SPARK_NAME FieldFx_FanSweepSpark
#define ROOM_FAN_SWEEP_SPARK_COLOR g_FieldFxFanSweepSparkTint
#define ROOM_FAN_SWEEP_SPARK_ANCHOR g_FanSweepSparkAnchor
#define ROOM_FAN_SWEEP_SPARK_TRAIL g_FanSweepSparkTrailPool
#define ROOM_FAN_SWEEP_SPARK_COS rcos
#define ROOM_FAN_SWEEP_SPARK_SIN rsin
#define ROOM_FAN_SWEEP_SPARK_RAND rand
#define ROOM_FAN_SWEEP_SPARK_PALETTE_LIT 7
#define ROOM_FAN_SWEEP_SPARK_PALETTE_BASE 3
#include "../../overlays/room_lib/fan_sweep_spark.inc"

/* Fan sweep controller: opens a spark pool and a trail pool, launches a
 * spark on a turning angle every other frame for 32 frames, updates the
 * trail for 70 frames and publishes its pool for the sparks.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
int FieldFx_FanSweep(int mode, FieldFanSweep *sweep)
{
    RoomFanSweepSpark *spark;
    int handles;

    switch (mode) {
    case 0:
        sweep->angle = rand();
        handles = FieldFx_InitCallbackList(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x10, 0x18,
                                (FieldFxCallbackListCallback)FieldFx_FanSweepSpark);
        handles += FieldFx_AllocCallbackList(&sweep->pool, handles, 8, 0x12, FieldFx_EmberSpark);
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
        if (g_FieldFxCurrentAge >= 0x46)
            return 1;
        FieldFx_UpdateCallbackList(sweep->pool);
        g_FanSweepSparkTrailPool = sweep->pool;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &g_ViewMatrixSlot.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        g_FieldFxSpriteParams.depth = 8;
        FieldFx_DrawCallbackList(sweep->pool);
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 1, &g_FanSweepSparkAnchor.x);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

/* Glow under the fan sweep: a pulsing sprite at the actor for 60 frames,
 * plus a flickering ring while the event is not in stage 11. */
int FieldFx_FanSweepGlow(int mode, FieldFanSweepGlow *glow)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = g_FieldFxFanSweepGlowTint;
    int intensity;
    int scale;
    int kind;
    int palette;
    int ring;

    switch (mode) {
    case 0:
        glow->state = 0;
        glow->timer = 0;
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 0, &glow->position.x);
        break;
    case 1:
        if (g_FieldFxCurrentAge >= 60)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 100;
        intensity = rsin((g_FieldFxCurrentAge << 11) / 60) / 32;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        scale = 0x2800;
        FieldFx_DrawShapeQuads(&glow->position, 0, scale, scale, 4,
                      GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 6 : palette + 2),
                      3, intensity, 0);
        if (FX_TASK_AS(FieldFanSweepEvent)->stage != 11) {
            position.x = glow->position.x;
            position.y = glow->position.y;
            position.z = glow->position.z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge * 8;
            rotation.flags = 0;
            scale = (g_FieldFxCurrentAge & 1) * 60 + 0x1000;
            ring = FX_TASK_AS(FieldFanSweepEvent)->stage == 10 ? 1 : 3;
            FieldFx_DrawRingBand(&position, 400, 500, 20, &rotation, scale, scale, 0,
                          &color, intensity, ring);
        }
        break;
    }
    return 0;
}

int FieldFx_ArcingSpark(int mode, RenderArcingEffect *state)
{
    RenderColor color = g_FieldFxArcingSparkTint;
    GteShortVector position;
    int palette;
    switch (mode) {
    case 1:
        state->y -= state->velocity_y;
        state->velocity_y--;
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        FieldFx_SampleColorTrack(g_ArcingEffectColorTrack, &color, g_FieldFxCurrentAge);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, 0, 4096, 4096, 0x8B,
                     GetClut(0x20, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_ArcingSparkEmitter(int mode, RenderArcingEmitter *state)
{
    RenderColor color;
    GteShortVector position;
    GteRotation rotation;
    RenderArcingEffect *effect;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 24,
                             (FieldFxTaskCallback)FieldFx_ArcingSpark);
    case 1:
        if (g_FieldFxCurrentAge < 51) {
            effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (effect) {
                effect->x = state->position.x + rcos(state->phase) * state->radius / 4096;
                effect->z = state->position.z + rsin(state->phase) * state->radius / 4096;
                effect->y = state->position.y;
                effect->velocity_y = rand() & 3;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (g_FieldFxCurrentAge >= 74)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 8;
        if (g_FieldFxCurrentAge < 51) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 1024;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 5;
            rotation.flags = 1;
            scale = rsin((g_FieldFxCurrentAge << 10) / 50);
            state->radius = scale / 8;
            FieldFx_SampleColorTrack(g_ArcingSparkColorTrack, &color, g_FieldFxCurrentAge);
            FieldFx_DrawRingBand(&position, 350, 500, 20, &rotation,
                          scale, scale, 0, &color, 128, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

int FieldFx_OrbitTrailPoint(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    int scale, clut_y;
    switch (mode) {
    case 1:
        state->y--;
        if (g_FieldFxCurrentAge >= 6)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        FieldFx_SampleColorTrack(g_OrbitTrailColorTrack, &color, g_FieldFxCurrentAge);
        scale = 4096 - (g_FieldFxCurrentAge << 12) / 6;
        clut_y = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            clut_y += 4;
        FieldFx_DrawShapeQuads(&position, 0, scale, scale, 0x8A,
                     GetClut(0x70, clut_y), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_OrbitingSprite(int mode, RenderOrbitingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = g_FieldFxOrbitHelicalTint;
    GteShortVector *particle;
    int scale, intensity, palette, interval;
    switch (mode) {
    case 1:
        state->x = g_OrbitingEffectAnchor.x + rcos(state->angle) * state->radius / 4096;
        state->z = g_OrbitingEffectAnchor.z + rsin(state->angle) * state->radius / 4096;
        state->angle += 96;
        state->radius = rcos((g_FieldFxCurrentAge << 10) / 36) * 700 / 4096;
        interval = g_FieldFxTaskContext->variables[6] == 11 ? 1 : 3;
        if (g_FieldFxCurrentAge % interval == 0) {
            particle = FieldFx_AcquireCallbackSlot(g_OrbitingEffectParticlePool);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (g_FieldFxCurrentAge >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            intensity = rsin((state->timer << 10) / 12) / 32;
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            intensity = 128;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            intensity = 128;
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[interval];
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 36,
                     GetClut(0, (interval == 4 && g_FieldFxAltPaletteFlag) ? palette + 7
                                                               : palette + 3),
                     1, intensity, &color);
        break;
    }
    return 0;
}

int FieldFx_OrbitingSpriteEmitter(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;

    switch (mode) {
    case 0:
        state->phase = rand();
        count = FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 12,
                              (FieldFxTaskCallback)FieldFx_OrbitingSprite);
        count += FieldFx_AllocCallbackList(&state->particles, count, 8, 32,
                               FieldFx_OrbitTrailPoint);
        return count;
    case 1:
        if (g_FieldFxCurrentAge < 44 && g_FieldFxCurrentAge % 3 == 0) {
            effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (effect) {
                phase = state->phase;
                effect->stage = 0;
                effect->timer = 0;
                effect->radius = 700;
                effect->angle = phase;
                FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
                effect->y = position.y + (rand() & 511) - 655;
                state->phase -= 0x555;
            }
        }
        if (g_FieldFxCurrentAge >= 70)
            return 1;
        FieldFx_UpdateCallbackList(state->particles);
        g_OrbitingEffectParticlePool = state->particles;
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[1]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        FieldFx_DrawCallbackList(state->particles);
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &g_OrbitingEffectAnchor.x);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[9]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int FieldFx_OrbitingSpriteBurst(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 3,
                             (FieldFxTaskCallback)FieldFx_OrbitingSprite);
        count += FieldFx_AllocCallbackList(&state->particles, count, 8, 18,
                                   FieldFx_OrbitTrailPoint);
        return count;
    case 1:
        if (g_FieldFxCurrentAge == 1) {
            for (count = 0; count < 3; ++count) {
                effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase;
                    FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
                    effect->y = position.y - 400;
                    state->phase -= 0x555;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 43)
            return 1;
        FieldFx_UpdateCallbackList(state->particles);
        g_OrbitingEffectParticlePool = state->particles;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &g_ViewMatrixSlot.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[1]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        FieldFx_DrawCallbackList(state->particles);
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &g_OrbitingEffectAnchor.x);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[9]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

int FieldFx_VerticalRingSprite(int mode, RenderVerticalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette, selector;
    switch (mode) {
    case 1:
        state->y = g_VerticalEffectAnchor.y + (-g_FieldFxCurrentAge * 900) / 24;
        state->x = g_VerticalEffectAnchor.x;
        state->z = g_VerticalEffectAnchor.z;
        state->angle += 128;
        state->timer++;
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        if (state->stage == 0) {
            scale = 8192 - rsin(state->timer << 7);
            if (state->timer >= 8) {
                state->timer = 0;
                state->stage = 1;
            }
        } else {
            scale = 4096;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 7;
        rotation.flags = 1;
        FieldFx_SampleColorTrack(g_VerticalEffectColorTrack, &color, g_FieldFxCurrentAge);
        scale *= 2;
        selector = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[selector];
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 68,
                     GetClut(0, (selector == 4 && g_FieldFxAltPaletteFlag) ? palette + 7
                                                               : palette + 3),
                     1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_VerticalEffectEmitter(int mode, void *state)
{
    GteShortVector position;
    RenderVerticalEffect *effect;
    switch (mode) {
    case 0:
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 12, 14,
                            (FieldFxTaskCallback)FieldFx_VerticalRingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 32 && (g_FieldFxCurrentAge & 1)) {
            effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (effect) {
                effect->angle = rand();
                effect->stage = 0;
                effect->timer = 0;
                FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
                effect->y = position.y - 512;
            }
        }
        if (g_FieldFxCurrentAge >= 50)
            return 1;
        /* Fall through: refresh the center and render parameters. */
    case 2:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &g_VerticalEffectAnchor.x);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

int FieldFx_HelicalTrailPoint(int mode, GteShortVector *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette;
    switch (mode) {
    case 1:
        state->y -= 3;
        if (g_FieldFxCurrentAge >= 12)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 6;
        rotation.flags = 0;
        FieldFx_SampleColorTrack(g_HelicalTrailColorTrack, &color, g_FieldFxCurrentAge);
        scale = 4096 - (g_FieldFxCurrentAge << 12) / 12;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 138,
                     GetClut(112, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_HelicalSprite(int mode, RenderHelicalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = g_FieldFxOrbitHelicalTint;
    GteShortVector *particle;
    int scale, palette, interval;
    switch (mode) {
    case 1:
        state->x = g_HelicalEffectAnchor.x + rcos(state->angle) * state->radius / 4096;
        state->z = g_HelicalEffectAnchor.z + rsin(state->angle) * state->radius / 4096;
        if (state->direction)
            state->y = g_HelicalEffectAnchor.y - g_FieldFxCurrentAge * 500 / 36;
        else
            state->y = (s16)(g_FieldFxCurrentAge * 500 / 36 - 1000) + g_HelicalEffectAnchor.y;
        state->angle += 96;
        state->radius = rcos((g_FieldFxCurrentAge << 10) / 36) * 700 / 4096;
        if ((g_FieldFxCurrentAge & 1) == 0) {
            particle = FieldFx_AcquireCallbackSlot(g_HelicalEffectParticlePool);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (g_FieldFxCurrentAge >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            rsin((state->timer << 10) / 12);
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[interval];
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 228,
                     GetClut(0, (interval == 4 && g_FieldFxAltPaletteFlag) ? palette + 6 : palette + 2),
                     1, 128, &color);
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int FieldFx_HelicalSpriteEmitter(int mode, RenderOrbitingEmitter *state)
{
    RenderHelicalEffect *effect;
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 4,
                             (FieldFxTaskCallback)FieldFx_HelicalSprite);
        count += FieldFx_AllocCallbackList(&state->particles, count, 8, 32,
                                   FieldFx_HelicalTrailPoint);
        return count;
    case 1:
        if (g_FieldFxCurrentAge < 2) {
            for (count = 0; count < 2; ++count) {
                effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase + (count << 11);
                    effect->direction = g_FieldFxCurrentAge;
                }
            }
            state->phase += 1024;
        }
        if (g_FieldFxCurrentAge >= 70)
            return 1;
        FieldFx_UpdateCallbackList(state->particles);
        g_HelicalEffectParticlePool = state->particles;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &g_ViewMatrixSlot.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[1]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        FieldFx_DrawCallbackList(state->particles);
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &g_HelicalEffectAnchor.x);
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[9]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

int FieldFx_DampedSpark(int mode, RenderDampedSpark *state)
{
    GteShortVector position;
    u32 color;
    switch (mode) {
    case 1:
        state->x += state->vx + (rand() & 3) - 1;
        state->y += state->vy + (rand() & 3) - 1;
        state->z += state->vz;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (g_FieldFxCurrentAge < 19)
            state->vy--;
        else
            state->vy++;
        if (g_FieldFxCurrentAge >= 40)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        if (g_FieldFxCurrentAge < 19)
            FieldFx_SampleColorTrack(g_DampedSparkColorTrack, &color, g_FieldFxCurrentAge);
        else if ((rand() & 3) == 0)
            color = 0xC8C8;
        else
            color = 0;
        FieldFx_DrawHaloedPoint(&position, &color, 128, 1);
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int FieldFx_DampedSparkEmitter(int mode, RenderSparkEmitter *state)
{
    GteRotation rotation;
    RenderColor color = g_FieldFxDampedSparkTint;
    RenderDampedSpark *effect;
    int count, speed, scale, intensity, palette, selector;
    switch (mode) {
    case 0:
        state->phase = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 12, 24,
                            (FieldFxTaskCallback)FieldFx_DampedSpark);
    case 1:
        if (g_FieldFxCurrentAge < 12) {
            for (count = 0; count < 2; ++count) {
                effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (effect) {
                    effect->x = state->position.x;
                    effect->y = state->position.y;
                    effect->z = state->position.z;
                    speed = (rand() & 7) + 43;
                    effect->vx = rsin(state->phase) * speed / 4096;
                    effect->vz = rcos(state->phase) * speed / 4096;
                    effect->vy = (rand() & 31) - 10;
                    state->phase += 0x955;
                }
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
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[9]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 100;
        if (g_FieldFxCurrentAge < 33) {
            intensity = rcos(g_FieldFxCurrentAge << 5) / 32;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 4;
            rotation.flags = 0;
            scale = rsin(g_FieldFxCurrentAge << 5) + 4096;
            selector = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[selector];
            FieldFx_DrawShapeQuads(&state->position, &rotation, scale, scale, 66,
                         GetClut(0, (selector == 4 && g_FieldFxAltPaletteFlag) ? palette + 7 : palette + 3),
                         1, intensity, &color);
        }
        {
            GteMatrix **slot;
            slot = &g_ViewMatrixSlot.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

#define SPINNING_MODEL_FUNC FieldFx_SpinningModelFlareA
#define SPINNING_MODEL_ANCHOR g_SpinningModelAnchorA
#define SPINNING_MODEL_PALETTE 4
#define SPINNING_MODEL_COLOR(size) 0, (u8)((size) / 2), (u8)((size) / 8)
#include "spinning_model.inc"

int FieldFx_RisingSpinSprite(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (g_FieldFxCurrentAge < 19)
            state->velocity_y -= 2;
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = g_FieldFxCurrentAge << 7;
        rotation.z = 0;
        rotation.flags = 1;
        intensity = rsin((g_FieldFxCurrentAge << 11) / 24) / 32;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, 4096, 4096, 136,
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int FieldFx_RisingSpinEmitter(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 12,
                            (FieldFxTaskCallback)FieldFx_RisingSpinSprite);
    case 1:
        if (g_FieldFxCurrentAge < 32 && (g_FieldFxCurrentAge & 1)) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x + rand() % 400 - 200;
                particle->y = state->y;
                particle->z = state->z + rand() % 400 - 200;
                particle->velocity_y = -(rand() & 7) - 16;
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
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

/* The second spinning model flare and the emitter that spawns it on the
 * attached actor's target point. */
#define SPINNING_MODEL_FUNC FieldFx_SpinningModelFlareB
#define SPINNING_MODEL_ANCHOR g_SpinningModelAnchorB
#define SPINNING_MODEL_PALETTE 3
#define SPINNING_MODEL_COLOR(size) (u8)((size) / 3), 0, (u8)((size) / 2)
#include "spinning_model.inc"

int FieldFx_SpinningModelEmitterB(int mode, FieldFxEmitter *state) {
    FieldFxEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F2D98, 0, 0);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 24,
                             (FieldFxCallbackListCallback)FieldFx_SpinningModelFlareB);
    case 1:
        if (g_FieldFxCurrentAge < 40 && g_FieldFxCurrentAge % 6 == 0) {
            point = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (point) {
                angle = state->angle;
                point->state = 0;
                point->phase = 0;
                point->angle = angle;
                state->angle -= 1365 + (rand() & 255);
            }
        }
        if (g_FieldFxCurrentAge >= 70)
            return 1;
        break;
    case 2:
        g_SpinningModelAnchorB[0] = g_FieldFxOwner->actor->render_object.target_x;
        g_SpinningModelAnchorB[1] = g_FieldFxOwner->actor->render_object.target_y;
        g_SpinningModelAnchorB[2] = g_FieldFxOwner->actor->render_object.target_z;
        g_SpinningModelAnchorB[1] = g_RoomFloorY->y;
        break;
    }
    return 0;
}

int FieldFx_DescendingSprite(int mode, RenderArcingEffect *state)
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
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int FieldFx_DescendingSpriteEmitter(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        state->x = g_FieldFxOwner->actor->render_object.target_x;
        state->y = g_FieldFxOwner->actor->render_object.target_y;
        state->z = g_FieldFxOwner->actor->render_object.target_z;
        state->y = g_RoomFloorY->y;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 12,
                             (FieldEffectCallback)FieldFx_DescendingSprite);
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
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 32;
        break;
    }
    return 0;
}
