#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_actor.h"
#include "pe1/field_fade_particle.h"
#include "pe1/field_flash_burst.h"
#include "pe1/field_glow_fountain.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_spin_glow.h"
#include "pe1/fieldfx_task.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"

int FieldFx_CosineEffect(int mode, RenderCosineEffect *effect) {
    u32 color;

    switch (mode) {
    case 1:
        effect->x = (effect->amplitude
            * rcos((g_FieldFxCurrentAge << 10) / effect->duration)) / 4096;
        effect->y += 16;
        if (g_FieldFxCurrentAge >= effect->duration)
            return 1;
        break;
    case 2:
        FieldFx_SampleColorTrack(g_CosineEffectColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawOrbitPoint(effect->x, effect->y, &color, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_CosineEmitter(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor first = g_FieldFxSparkOuterTint;
    RenderColor second = g_FieldFxSparkInnerTint;
    RenderCosineEffect *particle;
    int scale, intensity;
    switch (mode) {
    case 0:
        state->phase = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 24, (FieldFxTaskCallback)FieldFx_CosineEffect);
    case 1:
        if (g_FieldFxCurrentAge < 51) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->amplitude = (rand() & 255) + 500;
                particle->duration = (rand() & 3) + 22;
                particle->y = state->phase;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (g_FieldFxCurrentAge >= 80) return 1;
        break;
    case 2:
        if (g_FieldFxCurrentAge <= 80) {
            g_FieldFxSpriteParams.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = (g_FieldFxCurrentAge << 12) / 80;
            intensity = rsin((g_FieldFxCurrentAge << 11) / 80) / 32;
            FieldFx_DrawStarFan(&position, 800, 800, 16, 0, scale, scale, &first, 0, intensity, 1);
            FieldFx_DrawStarFan(&position, 250, 250, 8, 0, scale, scale, &second, 0, intensity, 1);
            FieldFx_DrawRingBand(&position, 590, 670, 16, 0, scale, scale, 0, &first, intensity, 3);
        }
        g_FieldFxSpriteParams.depth = 64;
        FieldFx_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}

int FieldFx_SineSweepTriangle(int mode, RenderSineEffect *state)
{
    RenderColor color;
    switch (mode) {
    case 1:
        state->position.y += state->velocity_y;
        state->position.z += 8;
        state->position.x = state->amplitude - state->amplitude * g_FieldFxCurrentAge / 64;
        state->angle = rsin(g_FieldFxCurrentAge << 5) / 64;
        state->scale = 1600 - g_FieldFxCurrentAge * 1000 / 64;
        if (g_FieldFxCurrentAge >= 64)
            return 1;
        break;
    case 2:
        FieldFx_SampleColorTrack(g_SineSweepColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawRotatedTriangle(&g_SineEffectAnchor, (GteRotation *)&state->position, state->scale, state->angle,
                     &color, 0, 0, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_SineEmitter(int mode, RenderSineEmitter *state)
{
    RenderSineEffect *effect;
    switch (mode) {
    case 0:
        state->phase = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 16,
                             (FieldFxTaskCallback)FieldFx_SineSweepTriangle);
    case 1:
        if (g_FieldFxCurrentAge < 17) {
            effect = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (effect) {
                effect->amplitude = (rand() & 1023) - 512;
                effect->position.y = state->phase;
                effect->position.z = rand();
                effect->velocity_y = (rand() & 127) - 64;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (g_FieldFxCurrentAge >= 73)
            return 1;
        break;
    case 2:
        g_SineEffectAnchor.x = state->position.x;
        g_SineEffectAnchor.y = state->position.y;
        g_SineEffectAnchor.z = state->position.z;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

void FieldFx_DrawDiamondOutline(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, RenderColor *color)
{
    GteShortVector center, p0, p1, p2, p3;
    RenderLinePacket *line;
    int r, g, b, dx, dy;
    line = (RenderLinePacket *)(D_800B08F8.packets[g_ActiveDrawSlot] + g_DrawPacketOffset);
    g_DrawPacketOffset += 64;
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_rtps();
    r = color->r * intensity / 128;
    g = color->g * intensity / 128;
    b = color->b * intensity / 128;
    gte_stsxy2(&center);
    center.x += x;
    center.y += y;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p0.x = center.x + dx;
    p0.y = center.y + dy;
    p2.x = center.x - dx;
    p2.y = center.y - dy;
    angle += 1024;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p1.x = center.x + dx;
    p1.y = center.y + dy;
    p3.x = center.x - dx;
    p3.y = center.y - dy;
#define EDGE(a, z) \
    SetLineF2(line); \
    line->r = r; line->g = g; line->b = b; \
    line->x0 = a.x; line->y0 = a.y; \
    line->x1 = z.x; line->y1 = z.y; \
    FieldFx_LinkBlendedPacket(D_800B08F8.ordering[g_ActiveDrawSlot] + 4, line, depth)
    EDGE(p0, p1);
    line++;
    EDGE(p1, p2);
    line++;
    EDGE(p2, p3);
    line++;
    EDGE(p3, p0);
#undef EDGE
}

int FieldFx_DiamondParticle(int mode, RenderDiamondParticle *state)
{
    RenderColor color;
    int fade;
    switch (mode) {
    case 1:
        if (g_FieldFxCurrentAge < 8) break;
        return 1;
    case 2:
        fade = 128 - (g_FieldFxCurrentAge << 4);
        FieldFx_SampleColorTrack(g_DiamondParticleColorTrack, &color, state->color_time);
        FieldFx_DrawDiamondOutline(&g_DiamondParticleAnchor, state->x, state->y, state->angle, state->size, 1, fade, &color);
        break;
    }
    return 0;
}

int FieldFx_DiamondEmitter(int mode, RenderDiamondEmitter *state)
{
    RenderColor color;
    RenderDiamondParticle *particle;
    int phase;
    switch (mode) {
    case 0:
        state->position.x = g_FieldFxOwner->actor->render_object.target_x;
        state->position.y = g_FieldFxOwner->actor->render_object.target_y;
        state->position.z = g_FieldFxOwner->actor->render_object.target_z;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 12, 10, (FieldFxTaskCallback)FieldFx_DiamondParticle);
    case 1:
        if (g_FieldFxCurrentAge < 25) {
            state->angle = -g_FieldFxCurrentAge * 64;
            state->radius = 150 - g_FieldFxCurrentAge * 150 / 24;
            state->x = rcos(state->angle * 2) * state->radius / 4096;
            state->y = rsin(state->angle * 2) * state->radius / 4096;
            state->size = rcos((g_FieldFxCurrentAge << 10) / 24) * 80 / 4096 + 32;
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x;
                particle->y = state->y;
                particle->angle = state->angle;
                particle->size = state->size;
            }
        } else {
            state->y = 0;
            state->x = 0;
        }
        if (g_FieldFxCurrentAge >= 72) return 1;
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);

        g_DiamondParticleAnchor.x = state->position.x;
        g_DiamondParticleAnchor.y = state->position.y;
        g_DiamondParticleAnchor.z = state->position.z;
        FieldFx_SampleColorTrack(g_DiamondParticleColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawDiamondOutline(&g_DiamondParticleAnchor, state->x, state->y, state->angle, state->size, 1, 128, &color);
        phase = g_FieldFxCurrentAge - 24;
        if ((unsigned)phase < 16) {
            int angle = phase * 64;
            FieldFx_DrawDiamondOutline(&g_DiamondParticleAnchor, state->x, state->y, state->angle, state->size + rcos(angle)/16, 1, 64, &color);
            FieldFx_DrawDiamondOutline(&g_DiamondParticleAnchor, state->x, state->y, state->angle, state->size + rcos(angle)/32, 1, 96, &color);
        }
        break;
    }
    return 0;
}

int FieldFx_RingFlash(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = g_SparkEmitterPhaseSeed;
        g_SparkEmitterPhaseSeed += 1365;
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
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
        FieldFx_SampleColorTrack(g_RingFlashColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawStarFan(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        FieldFx_DrawRingBand(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        FieldFx_SampleColorTrack(g_RingFlashInnerColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawStarFan(&position, 500, 100, 16, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_JitterSprite(int mode, GteShortVector *state)
{
    RenderColor color;
    GteRotation rotation;
    int scale, palette;
    switch (mode) {
    case 1:
        state->x += (rand() & 7) - 3;
        state->z += (rand() & 7) - 3;
        state->y += rand() & 3;
        if (g_FieldFxCurrentAge >= 32) return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge * 12;
        rotation.flags = 0;
        FieldFx_SampleColorTrack(g_JitterSpriteColorTrack, &color, g_FieldFxCurrentAge);
        scale = 6144 + (g_FieldFxCurrentAge << 7);
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        FieldFx_DrawShapeQuads(state, &rotation, scale, scale,
                     192 + (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 8 + 2),
                     GetClut(192, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_JitterSpriteEmitter(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        state->x = g_FieldFxOwner->actor->render_object.target_x;
        state->y = g_FieldFxOwner->actor->render_object.target_y;
        state->z = g_FieldFxOwner->actor->render_object.target_z;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 20,
                            (FieldFxTaskCallback)FieldFx_JitterSprite);
    case 1:
        if (g_FieldFxCurrentAge < 21) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x + (rand() & 511) - 256;
                particle->y = state->y + (rand() & 511) - 256;
                particle->z = state->z + (rand() & 511) - 256;
            }
        }
        if (g_FieldFxCurrentAge >= 53) return 1;
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
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

/* Matching debt: 3 GTE transfer pins, 4 empty constraints, and the three
 * TU-local register reservations above. The unpinned matrix pointer lets
 * GCC reuse t1 for constant-division multiply-high results. */
int FieldFx_TwinModelEffect(int mode, FieldFxTwinModel *state)
{
    GteShortVector angles;
    GteShortVector position;
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    int height;
    int spin;
    int size;

    switch (mode) {
    case 0:
        state->angle = rand();
        state->stage = 0;
        state->timer = 0;
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->position.x);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F2ED8, 0, 0);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_FieldFxTwinModelB, 0, 0);
        break;
    case 1:
        state->timer++;
        if (g_FieldFxCurrentAge >= 64)
            return 1;
        break;
    case 2:
        spin = 0;
        state->angle += 16;
        switch (state->stage) {
        case 0:
            height = (state->timer << 12) / 24;
            if (state->timer >= 24) {
                state->stage = 1;
                state->timer = 0;
            }
            break;
        case 1:
            height = 0x1000;
            spin = ((state->timer / 2 + 1) << 12) / 6;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            height = 0x1000;
            break;
        }
        scale.z = 0x44C;
        scale.x = 0x44C;
        scale.y = height / 4;
        angles.x = 0;
        angles.y = state->angle;
        angles.z = 0;
        FieldFx_SampleColorTrack(g_TwinModelColorTrack, &color, g_FieldFxCurrentAge);
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[9]];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        gte_ldrotmatrix((const GteMatrixWords *)g_ViewMatrixSlot.value);
        gte_ldtransmatrix((const GteMatrixWords *)g_ViewMatrixSlot.value);
        {
            int tpage = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlots[9]] | GetTPage(0, 3, 0, 0));
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_ModelSetTexture(tpage, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 10 : palette + 6));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_800F2ED8);
        FieldFx_ModelSetColors(D_800F2ED8, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F2ED8, &matrix);
        size = 0x4B0;
        scale.z = size;
        scale.x = size;
        angles.x = 0x800;
        angles.y = state->angle;
        angles.z = 0;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y -= 1000;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F2ED8, &matrix);
        FieldFx_ModelRestoreColors(D_800F2ED8);
        if (spin) {
            scale.z = size;
            scale.x = size;
            angles.x = 0;
            angles.y = state->angle + spin;
            angles.z = 0;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            RotMatrixYXZ(&angles, &matrix);
            ScaleMatrix(&matrix, &scale);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            {
                int tpage = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlots[9]] | GetTPage(0, 1, 0, 0));
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(tpage, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 10 : palette + 6));
            }
            FieldFx_ModelSetSemiTrans(1);
            FieldFx_ModelSaveColors(g_FieldFxTwinModelB);
            FieldFx_ModelSetColors(g_FieldFxTwinModelB, 255, 255, 255);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_FieldFxTwinModelB, &matrix);
            FieldFx_ModelRestoreColors(g_FieldFxTwinModelB);
        }
        break;
    }
    return 0;
}

int FieldFx_RisingJitterSprite(int mode, GteShortVector *state)
{
    RenderColor color;
    GteRotation rotation;
    int scale, palette;
    int selector;
    switch (mode) {
    case 1:
        state->x += (rand() & 7) - 3;
        state->z += (rand() & 7) - 3;
        state->y -= (rand() & 3) + 2;
        if (g_FieldFxCurrentAge >= 14) return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 7;
        rotation.flags = 0;
        FieldFx_SampleColorTrack(g_RisingJitterColorTrack, &color, g_FieldFxCurrentAge);
        scale = rsin((g_FieldFxCurrentAge << 11) / 14);
        selector = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[selector];
        FieldFx_DrawShapeQuads(state, &rotation, scale, scale, 226,
                      GetClut(0, (selector == 4 && g_FieldFxAltPaletteFlag) ? palette + 6
                                                              : palette + 2),
                      1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_RisingJitterEmitter(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 20,
                            (FieldFxTaskCallback)FieldFx_RisingJitterSprite);
    case 1:
        if (g_FieldFxCurrentAge < 40) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x + (rand() & 511) - 256;
                particle->y = state->y + (rand() & 511) - 256;
                particle->z = state->z + (rand() & 511) - 256;
            }
        }
        if (g_FieldFxCurrentAge >= 53) return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

int FieldFx_DriftingEffect(int mode, GteShortVector *position) {
    RenderColor color;
    GteRotation rotation;
    int scale;
    int palette;
    switch (mode) {
    case 1:
        position->x += (rand() & 7) - 3;
        position->z += (rand() & 7) - 3;
        position->y -= (rand() & 3) + 2;
        if (g_FieldFxCurrentAge >= 32)
            return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = g_FieldFxCurrentAge * 24;
        FieldFx_SampleColorTrack(g_DriftingEffectColorTrack, &color, g_FieldFxCurrentAge);
        scale = (g_FieldFxCurrentAge << 7) + 8192;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(position, &rotation, scale, scale, 130,
                      GetClut(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_DriftingEmitter(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 8, 20,
                            (FieldFxTaskCallback)FieldFx_DriftingEffect);
    case 1:
        if (g_FieldFxCurrentAge < 33 && (g_FieldFxCurrentAge & 1)) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->x = state->x + rand() % 800 - 400;
                particle->y = state->y + rand() % 400 - 700;
                particle->z = state->z + rand() % 800 - 400;
            }
        }
        if (g_FieldFxCurrentAge >= 60) return 1;
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
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

int FieldFx_TiltingSprite(int mode, RenderTiltingSprite *state)
{
    RenderColor color;
    int u, v, palette;
    switch (mode) {
    case 1:
        state->rotation.y += state->velocity_y;
        state->rotation.z += 8;
        state->rotation.x = state->initial_tilt - state->initial_tilt * g_FieldFxCurrentAge / 128;
        state->height = 114;
        state->width = rcos(g_FieldFxCurrentAge * 16) / 4 + 200;
        if (g_FieldFxCurrentAge >= 64) return 1;
        break;
    case 2:
        FieldFx_SampleColorTrack(g_TiltingSpriteColorTrack, &color, g_FieldFxCurrentAge);
        u = ((g_FieldFxCurrentAge / 2) & 3) * 64;
        v = (((g_FieldFxCurrentAge / 2) & 7) / 4) * 16 + 160;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        FieldFx_DrawRotatedQuad(&g_TiltingSpriteAnchor, &state->rotation, state->width, state->height,
                     u, v, 64, 16, GetClut(176, palette), 0, &color, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_TiltingSpriteEmitter(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    RenderTiltingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        state->phase = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 0, &state->position.x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 16,
                            (FieldFxTaskCallback)FieldFx_TiltingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 17) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->initial_tilt = (rand() & 511) - 256;
                particle->rotation.y = state->phase;
                particle->rotation.z = rand();
                particle->velocity_y = (rand() & 31) - 16;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (g_FieldFxCurrentAge >= 73) return 1;
        break;
    case 2:
        if (g_FieldFxCurrentAge < 65) {
            scale = (g_FieldFxCurrentAge << 6) + 2048;
            g_FieldFxSpriteParams.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            FieldFx_SampleColorTrack(g_TiltingSpriteColorTrack, &color, g_FieldFxCurrentAge);
            draw_mode = 1;
            if (g_FieldFxCurrentAge & 1) draw_mode = 3;
            FieldFx_DrawRingBand(&position, 200, 300, 16, 0, scale, scale, &color, 0, 128, draw_mode);
            scale = rsin(g_FieldFxCurrentAge * 16) / 2 + 2048;
            FieldFx_DrawStarFan(&position, 700, 700, 16, 0, scale, scale, &color, 0, 128, 1);
            FieldFx_DrawRingBand(&position, 600, 800, 24, 0, scale, scale, 0, &color, 128, 3);
        }
        g_TiltingSpriteAnchor.x = state->position.x;
        g_TiltingSpriteAnchor.y = state->position.y;
        g_TiltingSpriteAnchor.z = state->position.z;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

int FieldFx_StarFanRingGlow(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->position.x);
        state->position.y -= 510;
        return 0;
    case 1:
        if (g_FieldFxCurrentAge >= 64) return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 4096 + (g_FieldFxCurrentAge << 5);
        FieldFx_SampleColorTrack(g_StarFanRingGlowColorTrack, &color, g_FieldFxCurrentAge);
        FieldFx_DrawStarFan(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        FieldFx_DrawRingBand(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        FieldFx_SampleColorTrack(g_StarFanRingGlowSpinColorTrack, &color, g_FieldFxCurrentAge);
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = g_FieldFxCurrentAge * 16;
        FieldFx_DrawStarFan(&position, 400, 100, 24, &rotation, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_ConvergingSprite(int mode, RenderConvergingSprite *state)
{
    RenderColor color __attribute__((aligned(4)));
    GteRotation rotation;
    GteShortVector position;
    int scale, weight, palette;
    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2) return 1;
        break;
    case 2:
        scale = 0;
        switch (state->stage) {
        case 0:
            weight = (state->timer * 4096) / 52;
            LoadAverageShort12(&state->position, &g_ConvergingSpriteAnchor, 4096-weight, weight, &position);
            position.x += rsin(state->phase + g_FieldFxCurrentAge*160) / 16;
            state->position.y -= 2;
            FieldFx_SampleColorTrack(g_ConvergingSpriteColorTrack, &color, (g_FieldFxCurrentAge << 6) / 52);
            scale = rsin(g_FieldFxCurrentAge*128) / 2 + 4096;
            if (state->timer >= 52) {
                state->stage = 1;
                state->timer = 0;
                state->bursts = 0;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((g_FieldFxCurrentAge << 11) / 12);
            weight = scale / 32;
            /* The target writes all four packed color bytes in one word. */
            *(unsigned *)&color = (unsigned)weight | ((unsigned)weight << 8) | ((rand() & 15) << 16);
            if (state->timer >= 12) {
                state->bursts++;
                if (state->bursts < 4) {
                    state->position.x = g_ConvergingSpriteAnchor.x;
                    state->position.y = g_ConvergingSpriteAnchor.y;
                    state->position.z = g_ConvergingSpriteAnchor.z;
                    state->position.x += rand() % 400 - 200;
                    state->position.y += rand() % 400 - 200;
                    state->position.z += rand() % 400 - 200;
                    state->timer = 0;
                } else state->stage = 2;
            }
            break;
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = g_FieldFxCurrentAge * 12;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale, 128, GetClut(32,palette), 1, 128, &color);
        break;
    }
    return 0;
}

int FieldFx_ConvergingEmitter(int mode, RenderConvergingEmitter *state)
{
    RenderConvergingSprite *particle;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->position.x);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.y -= 550;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 24, (FieldFxTaskCallback)FieldFx_ConvergingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 49 && (g_FieldFxCurrentAge & 1)) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->position.x = state->position.x + rand() % 700 - 350;
                particle->position.y = state->position.y;
                particle->position.z = state->position.z + rand() % 700 - 350;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (g_FieldFxCurrentAge >= 140) return 1;
        break;
    case 2:
        g_ConvergingSpriteAnchor.x = state->target.x;
        g_ConvergingSpriteAnchor.y = state->target.y;
        g_ConvergingSpriteAnchor.z = state->target.z;
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 16;
        break;
    }
    return 0;
}

int FieldFx_SettlingSprite(int mode, RenderSettlingSprite *state)
{
    RenderColor color;
    GteShortVector position;
    int scale;
    int blend;
    int palette;

    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2)
            return 1;
        break;
    case 2:
        scale = 0x1000;
        switch (state->stage) {
        case 0:
            blend = 0x1000 - rcos((state->timer << 10) / 28);
            LoadAverageShort12(state, &g_SettlingSpriteAnchor, 0x1000 - blend, blend,
                               state);
            state->position.y += rsin(state->phase) / 128;
            state->phase += 160;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            FieldFx_SampleColorTrack(g_SettlingSpriteColorTrack, &color, (g_FieldFxCurrentAge << 6) / 28);
            scale = 0x2000 - (state->timer << 12) / 28;
            if (state->timer >= 28) {
                state->stage = 1;
                state->timer = 0;
                state->position.x += (rand() & 255) - 128;
                state->position.y += (rand() & 255) - 128;
                state->position.z += (rand() & 255) - 128;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            blend = rcos((state->timer << 10) / 12) / 32;
            *(u32 *)&color = blend | (blend << 8);
            state->position.x += (rand() & 7) - 3;
            state->position.y += (rand() & 7) - 3;
            state->position.z += (rand() & 7) - 3;
            if (state->timer >= 8)
                state->stage = 2;
            break;
        }
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        FieldFx_DrawHaloedPoint(&position, &color, 128, 1);
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, 0, scale, scale, 128, GetClut(32, palette), 3,
                      128, &color);
        break;
    }
    return 0;
}

int FieldFx_SettlingEmitter(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    RenderSettlingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &state->x);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 32,
                            (FieldFxTaskCallback)FieldFx_SettlingSprite);
    case 1:
        if (g_FieldFxCurrentAge < 67) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->position.x = state->x + (rand() & 511) - 256;
                particle->position.y = state->y - rand() % 600;
                particle->position.z = state->z + (rand() & 511) - 256;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (g_FieldFxCurrentAge >= 86) return 1;
        break;
    case 2:
        position.x = D_8009CD10->renderObject.matrices[19].translation[0];
        position.y = D_8009CD10->renderObject.matrices[19].translation[1];
        position.z = D_8009CD10->renderObject.matrices[19].translation[2];
        g_SettlingSpriteAnchor.x = position.x;
        g_SettlingSpriteAnchor.y = position.y;
        g_SettlingSpriteAnchor.z = position.z;
        if (g_FieldFxCurrentAge < 87) {
            scale = (g_FieldFxCurrentAge << 12) / 86 + 2048;
            g_FieldFxSpriteParams.depth = 16;
            FieldFx_SampleColorTrack(g_SettlingEmitterColorTrack, &color, g_FieldFxCurrentAge);
            draw_mode = 1;
            if (g_FieldFxCurrentAge & 1) draw_mode = 3;
            FieldFx_DrawStarFan(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
            color.b = 0;
            FieldFx_DrawRingBand(&position, 400, 300, 20, 0, scale, scale, &color, 0, 64, draw_mode);
        }
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

/* Mode 0 anchors at actor point 19; mode 1 runs 25 frames; mode 2 flashes
 * the screen, swells the model for 16 frames and draws the fan, band and
 * quad while the burst lasts. */
int FieldFx_FlashBurst(int mode, FieldFlashBurst *state)
{
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 1, state->point);
        state->anchor.x = g_PlayerEntity->render_object.matrices[19].translation[0];
        state->anchor.y = g_PlayerEntity->render_object.matrices[19].translation[1];
        state->anchor.z = g_PlayerEntity->render_object.matrices[19].translation[2];
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_FieldFxModel83ACD504, 0, 0);
        break;
    case 1:
        if (g_FieldFxCurrentAge >= 25)
            return 1;
        break;
    case 2:
        if (g_FieldFxCurrentAge == 0) {
            *(u32 *)&color = 0xA0A0A0;
            FieldFx_DrawScreenTint(&color.r, 0x50, 2, 8);
        }
        if (g_FieldFxCurrentAge >= 1 && g_FieldFxCurrentAge < 6) {
            *(u32 *)&color = 0xA0FFFF;
            FieldFx_DrawScreenTint(&color.r, 0x80 - (g_FieldFxCurrentAge - 1) * 32, 1, 8);
        }
        if (g_FieldFxCurrentAge < 17) {
            matrix = *(GteMatrix *)&g_PlayerEntity->render_object.matrices[0];
            scale.x = scale.y = rcos(g_FieldFxCurrentAge << 6) / 3;
            scale.z = rsin(g_FieldFxCurrentAge << 6);
            ScaleMatrix(&matrix, &scale);
            matrix.t[0] = g_PlayerEntity->render_object.matrices[19].translation[0];
            matrix.t[1] = g_PlayerEntity->render_object.matrices[19].translation[1];
            matrix.t[2] = g_PlayerEntity->render_object.matrices[19].translation[2];
            FieldFx_SampleColorTrack(g_FlashBurstColorTrack, &color, g_FieldFxCurrentAge);
            slots = &g_FieldFxTpageSlot1;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[slots[0]];
            g_FieldFxSpriteParams.palette = 1;
            FieldFx_EnsureEffectTexturePage(1);
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
                FieldFx_ModelSetTexture(tpage, GetClut(0xD0, palette));
            }
            FieldFx_ModelSetSemiTrans(1);
            FieldFx_ModelSaveColors(g_FieldFxModel83ACD504);
            FieldFx_ModelSetColors(g_FieldFxModel83ACD504, color.r, color.g, color.b);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_FieldFxModel83ACD504, &matrix);
            scale.x = scale.y = 0x1800;
            scale.z = 0x1000;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSetColors(g_FieldFxModel83ACD504, color.r, color.g >> 1, 0);
            {
                int tpage = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlot1] | GetTPage(0, 1, 0, 0));
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag)
                    palette += 4;
                FieldFx_ModelSetTexture(tpage, GetClut(0xD0, palette));
            }
            FieldFx_ModelDraw((union FieldTexturedModel *)g_FieldFxModel83ACD504, &matrix);
            FieldFx_ModelRestoreColors(g_FieldFxModel83ACD504);
        }
        if (g_FieldFxCurrentAge < 25) {
            g_FieldFxSpriteParams.depth = 16;
            FieldFx_SampleColorTrack(g_FlashBurstColorTrack, &color, (g_FieldFxCurrentAge << 4) / 24);
            FieldFx_DrawStarFan((GteShortVector *)state, 600, 600, 12, 0, 0x1000, 0x1000,
                          &color, 0, 0x80, 1);
            if (g_FieldFxCurrentAge < 17) {
                color.b = 0;
                size = rsin(g_FieldFxCurrentAge << 6) + 0x800;
                FieldFx_SampleColorTrack(g_FlashBurstColorTrack, &color, g_FieldFxCurrentAge);
                FieldFx_DrawRingBand((GteShortVector *)state, 500, 600, 20, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            intensity = rcos((g_FieldFxCurrentAge << 10) / 24) / 32;
            *(u32 *)&color = 0xC8B4B4;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot9];
            g_FieldFxSpriteParams.palette = 1;
            FieldFx_EnsureEffectTexturePage(1);
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads((GteShortVector *)state, 0, 0x2000, 0x2000, 0x42,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 7 : palette + 3),
                              1, intensity, &color);
            }
        }
        break;
    }
    return 0;
}

/* Mode 1 moves the glow with damped horizontal speed and gravity for 32
 * ticks; mode 2 draws a pulsing spinning sprite with a fainter halo. */
int FieldFx_SpinGlow(int mode, FieldSpinGlow *state)
{
    GteShortVector position;
    GteRotation rotation;
    int scale;
    int intensity;
    int palette;

    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vy = state->vy + 1;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (g_FieldFxCurrentAge >= 32)
            return 1;
        break;
    case 2:
        intensity = state->intensity - state->intensity * g_FieldFxCurrentAge / 32;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + g_FieldFxCurrentAge * 8;
        scale = rsin(g_FieldFxCurrentAge * 32) + 0x2800;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 4) + 0xC0, GetClut(64, palette), 1,
                      intensity, 0);
        scale = 0x1800;
        palette = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
        if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
            palette += 4;
        FieldFx_DrawShapeQuads(&position, &rotation, scale, scale,
                      (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 8) + 0x60, GetClut(0, palette), 3,
                      intensity, 0);
        break;
    }
    return 0;
}

/* Mode 0 anchors at the owner actor and opens a pool of 32 spin glows;
 * mode 1 throws two glows per frame for 16 frames and ends at frame 48;
 * mode 2 draws the rising shape quad, the opening ring and the band. */
int FieldFx_GlowFountain(int mode, FieldGlowFountain *state)
{
    GteRotation rotation;
    GteShortVector vector;
    RenderColor tint = g_FieldFxGlowFountainTint;
    FieldSpinGlow *glow;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->seed = rand();
        state->x = g_FieldFxOwner->actor->render_object.target_x;
        state->y = g_FieldFxOwner->actor->render_object.target_y;
        state->z = g_FieldFxOwner->actor->render_object.target_z;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 32,
                             (FieldEffectCallback)FieldFx_SpinGlow);
    case 1:
        if (g_FieldFxCurrentAge < 16) {
            glow = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (glow) {
                for (i = 0; i < 2; i++) {
                    vector.x = rand() % 48 - 24;
                    vector.z = -(rand() % 80);
                    vector.y = rand() % 48 - 32;
                    FieldFx_RotateVector(
                        (GteMatrixWords *)g_PlayerEntity->render_object.matrices,
                        &vector, &vector);
                    glow->vx = vector.x;
                    glow->vy = vector.y;
                    glow->vz = vector.z;
                    glow->x = state->x + (rand() & 0xFF) - 0x80;
                    glow->y = state->y + (rand() & 0xFF) - 0x80;
                    glow->z = state->z + (rand() & 0xFF) - 0x80;
                    glow->intensity = (rand() & 0x3F) + 0x50;
                    glow->angle = rand();
                    state->seed += 0x955;
                }
            }
        }
        if (g_FieldFxCurrentAge >= 48)
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
        g_FieldFxSpriteParams.parameter0A = 5;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.depth = 0x40;
        if (g_FieldFxCurrentAge < 25) {
            state->y -= 6;
            intensity = rcos((g_FieldFxCurrentAge << 10) / 24) * 240 / 4096;
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = g_FieldFxCurrentAge << 4;
            scale = 0x3000 - rcos((g_FieldFxCurrentAge << 10) / 12);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)state, &rotation, scale, scale, 0x40,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, intensity, 0);
            if (g_FieldFxCurrentAge < 9) {
                intensity = rcos(g_FieldFxCurrentAge << 7) / 32;
                FieldFx_DrawStarFan((GteShortVector *)state, 100, 700, 16, &rotation,
                              scale, scale, &tint, 0, intensity, 1);
            }
        }
        g_FieldFxSpriteParams.depth = 0;
        if (g_FieldFxCurrentAge < 17) {
            intensity = 0x80 - (g_FieldFxCurrentAge - 4) * 8;
            vector.x = state->x;
            vector.y = state->y;
            vector.z = state->z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 7;
            scale = rsin((g_FieldFxCurrentAge - 4) << 6) * 2;
            FieldFx_DrawRingBand(&vector, 600, 700, 0x18, &rotation, scale, scale, 0,
                          &tint, intensity, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FieldFxSpriteParams.palette = 1;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0x18;
        break;
    }
    return 0;
}

/* Phase 0 follows actor matrix point `kind` and every fourth frame sheds a
 * falling spark (phase 2, one in three) or a drifting one (phase 1);
 * phases 1 and 2 drift for 16 and 24 frames. Mode 2 draws each phase as a
 * shape quad capped at the emitter's intensity g_FadeEmitterIntensity. */
int FieldFx_FadeParticle(int mode, RenderFadeParticle *particle)
{
    GteShortVector offset = g_FieldFxZeroOffset;
    RenderColor color;
    RenderFadeParticle *child;
    int scale;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (particle->phase) {
        case 0:
            particle->timer++;
            FieldFx_TransformMatrixPoint((struct RoomFxTransformOwner *)g_PlayerEntity,
                                          particle->kind, &offset,
                                          (GteShortVector *)particle);
            if (g_FieldFxCurrentAge & 3)
                return 0;
            if (rand() % 3 == 0) {
                child = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (child == 0)
                    break;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->vx = (rand() & 7) - 3;
                child->vz = (rand() & 7) - 3;
                child->vy = (rand() & 0xF) - 12;
                child->phase = 2;
                child->timer = 0;
            } else {
                child = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
                if (child == 0)
                    return 0;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->position[0] += (rand() & 0x7F) - 0x40;
                child->position[1] += (rand() & 0x7F) - 0x40;
                child->position[2] += (rand() & 0x7F) - 0x40;
                child->vx = (rand() & 0xF) - 7;
                child->vz = (rand() & 0xF) - 7;
                child->vy = (rand() & 7) - 11;
                child->phase = 1;
                child->timer = 0;
            }
            break;
        case 1:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            particle->position[0] += (rand() & 7) - 3;
            particle->position[1] += (rand() & 7) - 3;
            particle->position[2] += (rand() & 7) - 3;
            if (particle->timer < 16)
                break;
            return 1;
        case 2:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            if (particle->timer < 24)
                break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (particle->phase) {
        case 0:
            scale = 0x1800;
            if (particle->timer < 33)
                scale = rsin(particle->timer << 5) + 0x800;
            if (particle->timer < 25) {
                intensity = rsin((particle->timer << 10) / 24) / 32;
            } else if (particle->timer & 1) {
                intensity = 0x80;
            } else {
                intensity = 0x64;
            }
            if (g_FadeEmitterIntensity < intensity)
                intensity = g_FadeEmitterIntensity;
            FieldFx_DrawShapeQuads((GteShortVector *)particle, 0, scale / 2, scale / 2,
                          (s16)g_FieldFxSpriteParams.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
                                      ? g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette] + 4
                                      : g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette]),
                          1, intensity, 0);
            break;
        case 1:
            intensity = particle->timer << 6;
            scale = rcos(intensity) / 2 + 0x800;
            intensity = rcos(intensity) / 32;
            if (g_FadeEmitterIntensity < intensity)
                intensity = g_FadeEmitterIntensity;
            FieldFx_DrawShapeQuads((GteShortVector *)particle, 0, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag)
                                      ? g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette] + 4
                                      : g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette]),
                          1, intensity / 2, 0);
            break;
        case 2:
            scale = rsin((particle->timer << 10) / 24) + 0x1000;
            FieldFx_SampleColorTrack(g_FadeParticleColorTrack, &color, particle->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag)
                palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)particle, 0, scale / 2, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * (s16)(particle->timer / 6) + 0x60,
                          GetClut(0, palette), 1, 0x80, &color);
            break;
        }
        break;
    }
    return 0;
}

int FieldFx_FadeEmitter(int mode, RenderFadeEmitter *state)
{
    RenderFadeParticle *particle;
    u16 intensity, page;
    switch (mode) {
    case 0:
        state->count = 0;
        state->intensity = 160;
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 20, 32,
                             (FieldFxCallbackListCallback)FieldFx_FadeParticle);
    case 1:
        if ((g_FieldFxCurrentAge & 3) == 0 && state->count < 8) {
            particle = FieldFx_AcquireCallbackSlot(g_FieldFxActiveTaskSlot->end);
            if (particle) {
                particle->kind = g_FadeEmitterParticleKinds[state->count];
                particle->phase = 0;
                particle->timer = 0;
            }
            state->count++;
        }
        if (g_FieldFxCurrentAge >= 32)
            state->intensity -= 2;
        if (state->intensity <= 0) {
            state->intensity = 0;
            return 1;
        }
        break;
    case 2:
        intensity = state->intensity;
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        page = g_FieldFxTpageTable[g_FieldFxTpageSlot1];
        g_FadeEmitterIntensity = intensity;
        g_FieldFxSpriteParams.palette = 1;
        g_FieldFxSpriteParams.tpage = page;
        FieldFx_EnsureEffectTexturePage(1);
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 24;
        break;
    }
    return 0;
}

int FieldFx_RotatingEffect(int mode) {
    GteShortVector position;
    GteRotation rotation;
    u32 color;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (g_FieldFxCurrentAge >= 24)
            return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 0;
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, &position.x);
        position.y -= 300;
        FieldFx_SampleColorTrack(g_RotatingEffectColorTrack, &color, g_FieldFxCurrentAge);
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = g_FieldFxCurrentAge << 8;
        rotation.flags = 1;
        scale = rsin((g_FieldFxCurrentAge << 10) / 24);
        FieldFx_DrawRingBand(&position, 300, 400, 10, &rotation,
                      scale, scale, 0, (RenderColor *)&color, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_PointEmitter(int mode, FieldFxEmitter *state) {
    FieldFxEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F2D98, 0, 0);
        return FieldFx_InitCallbackList(g_FieldFxActiveTaskSlot->end, 16, 24,
                             (FieldFxCallbackListCallback)FieldFx_SpinningModelFlareA);
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
        FieldFx_GetActorPosition((char *)D_8009CD10, 1, g_SpinningModelAnchorA);
        break;
    }
    return 0;
}
