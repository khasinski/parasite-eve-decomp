/*
 * The sound burst: a particle that swings up and lights the battle actor,
 * and the controller that spawns it and plays the burst's sound. The two
 * functions follow each other in museum rooms m205, m231, m234 and m243 and
 * Chrysler rooms m406 and m410; the unit's rodata is the particle's colour
 * and the controller's spawn template.
 */

#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/room_sound_burst.h"

/* Particle emitted by the sound burst controller: it swings on a sine
 * while rising, lights the battle actor when it lands near it, and draws a
 * cosine-sized sprite with its floor shadow. */
int RoomEffect_SoundBurstParticle(int mode, RoomSoundBurstParticle *particle,
                                  s32 *step) {
    GteShortVector position;
    RoomSoundBurstColor color = {0x40, 0x00, 0x80, 0x00};
    GteShortVector *world;
    int swing;
    int scale;
    int base;
    int size;
    u16 kind;
    int palette;
    int clut;
    int clut2;

    switch (mode) {
    case 1:
        particle->swing += step[0];
        particle->amplitude += step[1];
        particle->phase += step[2];
        swing = rsin((s16)particle->phase) * (s16)particle->amplitude / 4096;
        world = (GteShortVector *)&particle->wx;
        position.x = 0;
        position.y = particle->height;
        position.z = 0;
        FieldFx_OffsetPointByAngles(particle, world, (s16)particle->swing, &position, swing,
                      0);
        if (FieldFx_IsPlayerWithinRadius(world, 100) != 0) {
            if (FX_TASK_AS(RoomSoundBurstEventState)->active) {
                RoomSoundBurstChannel *channel = ANIM_OWNER_AS(RoomSoundBurstChannel);
                u32 *entry;
                if ((**(u32 **)channel->pool & 0x3F000000) == 0x01000000) {
                    ((RoomSoundBurstBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                    entry = *(u32 **)channel->pool;
                    *entry = (*entry & 0xC0FFFFFF) | 0x21000000;
                    entry = *(u32 **)channel->pool;
                    *entry |= 0x80000000;
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x40) break;
        return 1;
    case 2:
        scale = rcos(g_FieldFxCurrentAge << 4) / 32;
        scale = particle->spread * scale / 128;
        base = (g_FieldFxCurrentAge << 8) + 0x1000;
        size = (rcos(particle->tilt) / 4 + 0x1000) * base / 4096;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x80, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)&particle->wx, 0, size, size,
                      (s16)g_FieldFxSpriteParams.parameter02 * ((g_FieldFxCurrentAge / 4) & 1) + 0xFD, clut, 1,
                      scale, (struct RenderColor *)&color);
        position.x = 0x400;
        position.y = 0;
        position.z = 0;
        position.pad = 1;
        particle->wy = g_RoomFloorY->y;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut2 = GetClut(0x80, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)&particle->wx, (GteRotation *)&position, size, size,
                      (s16)g_FieldFxSpriteParams.parameter02 * ((g_FieldFxCurrentAge / 4) & 1) + 0xFD, clut2, 2,
                      scale / 4, 0);
        break;
    }
    return 0;
}

int RoomEffect_SoundBurstController(int mode, RoomSoundBurstState *state) {
    RoomSoundBurstTemplate8 template = {{0x00000000, 0x0000FFC4}};
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomSoundBurstParticle *child;
    int angle;

    switch (mode) {
    case 0:
        state->soundHandle = FieldFx_PlaySoundAtOwner(0x5A1, FieldFx_GetOwnerProgress());
        pool = ANIM_POOL_AS(RoomSoundBurstChannel)->pool;
        return FieldFx_InitCallbackList(pool, 24, 24,
                             (FieldFxCallbackListCallback)RoomEffect_SoundBurstParticle);
    case 1:
        state->frame++;
        if (g_FieldFxCurrentAge < 0x47) {
            pool = ANIM_OWNER_AS(RoomSoundBurstChannel)->pool;
            FieldFx_TransformMatrixPoint(pool, 9, &template, position);
            angle = rsin((g_FieldFxCurrentAge << 11) / 70) / 32;
            if (g_FieldFxCurrentAge % 3 == 0) {
                pool = ANIM_POOL_AS(RoomSoundBurstChannel)->pool;
                child = FieldFx_AcquireCallbackSlot(pool);
                if (child) {
                    pool = ANIM_OWNER_AS(RoomSoundBurstChannel)->pool;
                    FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)pool, 0,
                                  (GteShortVector *)target);
                    child->height = target[1];
                    child->x = position[0];
                    child->y = position[1];
                    child->z = position[2];
                    child->phase = 0;
                    child->amplitude = 0;
                    child->swing = 0;
                    child->tilt = rand();
                    child->spread = angle;
                }
                if (FX_TASK_AS(RoomSoundBurstEventState)->active) {
                    RoomSoundBurstNode **slot =
                        (RoomSoundBurstNode **)ANIM_OWNER_AS(RoomSoundBurstChannel)->pool;
                    if (slot && *slot) {
                        u8 *flag = (*slot)->state;
                        if (*flag == 1) *flag = 2;
                    }
                }
            }
        }
        if (g_FieldFxCurrentAge == 0x46 && state->soundHandle != -1) {
            Akao_Cmd_21(state->soundHandle, 0);
        }
        if (g_FieldFxCurrentAge < 8) break;
        return 2;
    case 2:
    {
        int idx = g_FieldFxTpageSlot2;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 16;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 16;
        g_FieldFxSpriteParams.extent_y = 16;
        palette = g_FieldFxTpageTable[idx];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = palette;
        break;
    }
    }
    return 0;
}

#undef RoomEffect_SoundBurstParticle
