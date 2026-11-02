#include "common.h"
#include "pe1/field_model_draw.h"
#include "pe1/gte.h"
#include "pe1/room_m089_model_pulse.h"
#include "pe1/room_m089_spin_model.h"
#include "pe1/room_m089_trail.h"
#include "pe1/room_sound_slot.h"
#include "pe1/room_swirl_rise.h"

#include "room_m089.h"

/* Spiral trail: the history shift is written out field by field (retail
 * loads all fifteen halfwords before the first store), and the flash angle
 * is `timer * 128`, which keeps the signed lh that `timer << 7` loses. */

int RoomM089_SpiralTrailCallback(int mode, RoomM089SpiralTrail *trail, GteShortVector *anchor) {
    GteRotation rotation;
    RenderColor head = g_RoomM089TrailHeadColor;
    RenderColor tail = g_RoomM089TrailTailColor;
    RenderMatrixSlot *matrixSlot;
    int radius;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer++;
            trail->points[5].x = trail->points[4].x;
            trail->points[5].y = trail->points[4].y;
            trail->points[5].z = trail->points[4].z;
            trail->points[4].x = trail->points[3].x;
            trail->points[4].y = trail->points[3].y;
            trail->points[4].z = trail->points[3].z;
            trail->points[3].x = trail->points[2].x;
            trail->points[3].y = trail->points[2].y;
            trail->points[3].z = trail->points[2].z;
            trail->points[2].x = trail->points[1].x;
            trail->points[2].y = trail->points[1].y;
            trail->points[2].z = trail->points[1].z;
            trail->points[1].x = trail->points[0].x;
            trail->points[1].y = trail->points[0].y;
            trail->points[1].z = trail->points[0].z;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            trail->points[0].y -= trail->rise - trail->rise * trail->timer / 16;
            radius = trail->points[0].pad - trail->points[0].pad * trail->timer / 16;
            trail->points[0].x += rcos(trail->angle) * radius / 4096;
            trail->points[0].z += rsin(trail->angle) * radius / 4096;
            trail->angle += trail->points[1].pad;
            if (trail->timer >= 16) {
                trail->state = 1;
                trail->timer = 0;
            }
            break;
        case 1:
            trail->timer++;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            if (trail->timer >= 8) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (trail->state) {
        case 0:
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            if (trail->points[5].x | trail->points[5].y | trail->points[5].z)
                FieldFx_DrawGlowLine(&trail->points[4], &trail->points[5], &head, &tail, 0x19, 0, 1);
            if (trail->points[4].x | trail->points[4].y | trail->points[4].z)
                FieldFx_DrawGlowLine(&trail->points[3], &trail->points[4], &head, &tail, 0x32, 0x19, 1);
            if (trail->points[3].x | trail->points[3].y | trail->points[3].z)
                FieldFx_DrawGlowLine(&trail->points[2], &trail->points[3], &head, &tail, 0x4B, 0x32, 1);
            if (trail->points[2].x | trail->points[2].y | trail->points[2].z)
                FieldFx_DrawGlowLine(&trail->points[1], &trail->points[2], &head, &tail, 0x64, 0x4B, 1);
            if (trail->points[1].x | trail->points[1].y | trail->points[1].z)
                FieldFx_DrawGlowLine(&trail->points[0], &trail->points[1], &head, &tail, 0x80, 0x64, 1);
            break;
        case 1:
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = trail->timer * 128;
            rotation.flags = 1;
            intensity = rcos(trail->timer << 7) / 32;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&trail->points[0], &rotation, 0x1000, 0x1000, 6,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, intensity, &head);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

int RoomM089_SpiralTrail(int mode, unsigned short *dst, unsigned short *src) {
    short *effect;
    RoomChanCtx *channel;
    int random;

    switch (mode) {
    case 0:
    channel = ANIM_POOL_AS(RoomChanCtx);
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    return FieldFx_InitCallbackList((char *)channel->w8, 0x38, 8,
                         (FieldFxCallbackListCallback)RoomM089_SpiralTrailCallback);

    case 1:
    if (g_FieldFxCurrentAge < 9) {
        effect = FieldFx_AcquireCallbackSlot((char *)ANIM_POOL_AS(RoomChanCtx)->w8);
        if (effect != 0) {
            effect[2] = 0;
            effect[1] = 0;
            effect[0] = 0;
            effect[6] = 0;
            effect[5] = 0;
            effect[4] = 0;
            effect[10] = 0;
            effect[9] = 0;
            effect[8] = 0;
            effect[14] = 0;
            effect[13] = 0;
            effect[12] = 0;
            effect[18] = 0;
            effect[17] = 0;
            effect[16] = 0;
            effect[22] = 0;
            effect[21] = 0;
            effect[20] = 0;
            effect[25] = 0;
            effect[24] = 0;
            effect[26] = rand();
            effect[3] = (rand() & 0x3FF) + 0x400;
            if (rand() & 1) {
                effect[7] = (rand() & 0x7F) + 0x1AC;
            } else {
                effect[7] = -0x1AC - (rand() & 0x7F);
            }
            effect[27] = (rand() & 0x3FF) + 0x5DC;
        }
    }

    if (g_FieldFxCurrentAge == 9) {
        RoomSoundSlot *sound = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
        int volume;
        if (sound->channel != 0) {
            volume = 0x7F;
            random = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(sound->channel, 0x5BB, random, 0x80, volume);
            if (sound->channel != 0) {
                Akao_SendTableCommand(sound->channel, 0x5BC, 0x80, 0x80, volume);
            }
        }
    }
    if (g_FieldFxCurrentAge < 0x24) {
        break;
    }
    return 1;

    case 2:
    {
        int value;
        int index;
        index = g_FieldFxTpageSlot11;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        value = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        g_FieldFxSpriteParams.tpage = value;
    }
        break;
    default:
        break;
    }
    return 0;
}

/* Spinning model: loads its model, swells a glow over the anchor for two
 * stages and draws the model twice, the inner copy at half scale. The tile
 * index and tpage table go through local pointers (retail keeps both in
 * saved registers, which spills the model intensity to the frame), and the
 * palette kind/palette pair is block-local per draw. */

int RoomM089_SpinModel(int mode, RoomM089SpinModel *spin, GteShortVector *anchor) {
    GteShortVector position;
    GteShortVector rotation;
    RenderColor color = g_RoomM089SpinGlowColor;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix innerMatrix;
    GteVector innerScale;
    int intensity;
    int size;
    int glow;
    int turn;
    int page;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 0:
        spin->stage = 0;
        spin->timer = 0;
        g_RoomM089CoreModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5864704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM089CoreModel, 0, 0);
        return 0;
    case 1:
        spin->timer++;
        if (spin->stage < 2) break;
        return 1;
    case 2:
        size = 0;
        glow = 0;
        turn = 0;
        intensity = 0;
        switch (spin->stage) {
        case 0:
            turn = rcos(spin->timer << 6);
            intensity = rsin(spin->timer << 7) / 32;
            glow = 0x40;
            size = rsin(spin->timer << 6);
            if (spin->timer >= 16) {
                spin->stage = 1;
                spin->timer = 0;
            }
            break;
        case 1:
            glow = rcos(spin->timer << 6) / 32 * 3 / 2;
            size = 0x1000;
            if (spin->timer >= 16) {
                spin->stage = 2;
                spin->timer = 0;
            }
            break;
        }
        index = &g_FieldFxTpageSlot11;
        tpages = g_FieldFxTpageTable;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 2;
        position.x = anchor->x;
        position.y = anchor->y;
        position.z = anchor->z;
        if (size != 0) {
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = -g_FieldFxCurrentAge << 6;
            rotation.pad = 1;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&position, (GteRotation *)&rotation, size, size, 0x26,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 9 : palette + 5),
                              1, glow, &color);
            }
        }
        rotation.x = 0;
        rotation.z = 0;
        position.y -= (0x20 - g_FieldFxCurrentAge) << 4;
        turn = turn * 2 / 3;
        rotation.y = -(g_FieldFxCurrentAge << 6);
        g_FieldFxSpriteParams.tpage = tpages[*index];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 7 : palette + 3));
        }
        FieldFx_ModelSetSemiTrans(1);
        RotMatrixYXZ(&rotation, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = turn;
        scale.y = turn;
        scale.z = turn;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelSaveColors(g_RoomM089CoreModel);
        FieldFx_ModelScaleColors(g_RoomM089CoreModel, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089CoreModel, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM089CoreModel);
        position.y += (0x20 - g_FieldFxCurrentAge) << 3;
        RotMatrixYXZ(&rotation, &innerMatrix);
        innerMatrix.t[0] = position.x;
        innerMatrix.t[1] = position.y;
        innerMatrix.t[2] = position.z;
        innerScale.x = turn / 2;
        innerScale.y = turn / 2;
        innerScale.z = turn / 2;
        ScaleMatrix(&innerMatrix, &innerScale);
        FieldFx_ModelSaveColors(g_RoomM089CoreModel);
        FieldFx_ModelScaleColors(g_RoomM089CoreModel, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089CoreModel, &innerMatrix);
        FieldFx_ModelRestoreColors(g_RoomM089CoreModel);
        break;
    }
    return 0;
}

/* Swirl/rise sprite for room_m089. Both draws pass their intensity through
 * one function-scope variable (two sets keep each computation where retail
 * has it), and the second draw takes the state as its rotation through a
 * local pointer, whose copy keeps the frame counter load below the palette
 * lookup. */

int RoomM089_SwirlRiseSprite(int mode, RoomSwirlRiseState *state,
                                        GteShortVector *anchor) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int intensity;

    color = g_RoomM089SwirlSpriteColor;
    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0: {
            int radius;

            state->x = anchor->x;
            state->y = anchor->y;
            state->z = anchor->z;
            radius = state->radius * g_FieldFxCurrentAge / 64;
            state->x += rcos(state->angle) * radius / 4096;
            state->z += rsin(state->angle) * radius / 4096;
            state->y -= rsin(g_FieldFxCurrentAge << 5) * 120 / 4096;
            state->angle += 12;
            if (g_FieldFxCurrentAge >= 64) {
                return 1;
            }
            break;
        }
        case 1:
            state->x += 0x20;
            if (g_FieldFxCurrentAge >= 6) {
                return 1;
            }
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0: {
            int scale;
            int kind;
            int palette;
            int clut;

            rotation.x = 0;
            rotation.y = 0;
            rotation.z = state->angle + g_FieldFxCurrentAge * 12 + state->radius;
            rotation.flags = 0;
            intensity = 0x80 - g_FieldFxCurrentAge * 2;
            scale = rsin(g_FieldFxCurrentAge << 4) + 0x800;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x20, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)state, &rotation, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * (g_FieldFxCurrentAge / 16) + 0x24, clut, 1,
                          intensity, 0);
            return 0;
        }
        case 1: {
            int v;
            GteRotation *spin;
            int kind;
            int palette;
            u16 clut;

            origin.x = anchor->x;
            origin.y = anchor->y;
            origin.z = anchor->z;
            intensity = rcos((g_FieldFxCurrentAge << 10) / 6) / 32;
            *(int *)&color = 0x808080;
            spin = (GteRotation *)state;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            v = ((g_FieldFxCurrentAge & 1) << 4) + 0x80;
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x10, palette);
            FieldFx_DrawRotatedQuad(&origin, spin, 0x3C0, 0x8C, 0, v, 0xFF,
                          0xF, clut, &color, 0, (s16)intensity, 1);
            break;
        }
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

/* Pulsing model burst: spawns a ring of swirl sprites on the first frame and
 * rising sprites every other frame, tags the player while it is close, and
 * draws three turning layers of one model, a fourth model and a glow strip
 * that fade out over the first 0x31 frames. */
int RoomM089_ModelPulse(int mode, RoomM089ModelPulse *state, GteShortVector *anchor) {
    GteShortVector rotation;
    RenderColor color;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix wideMatrix;
    GteVector wideScale;
    GteMatrix lowMatrix;
    GteVector lowScale;
    GteMatrix coreMatrix;
    RoomSwirlRiseState *child;
    RoomM089PulseChannel *channel;
    int fade;
    int outer;
    int tall;
    int glow;
    int wide;
    int low;
    int page;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->timer = 0;
        state->position.x = anchor->x;
        state->position.y = anchor->y;
        state->position.z = anchor->z;
        if (FX_TASK_AS(RoomM089PulseEvent)->active) {
            RoomM089PulsePool *pool = ANIM_OWNER_AS(RoomM089PulseChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        g_RoomM089PulseModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5464704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM089PulseModel, 0, 0);
        g_RoomM089CoreModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5864704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM089CoreModel, 0, 0);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM089SpawnChannel)->pool, 0x10, 0x18,
                             (FieldFxCallbackListCallback)RoomM089_SwirlSpriteCallback);
    case 1:
        if (g_FieldFxCurrentAge == 1) {
            for (i = 0; i < 16; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM089SpawnChannel)->pool);
                if (child) {
                    child->phase = 0;
                    child->angle = (rand() & 0x7F) + (i << 8);
                    child->radius = (rand() & 0xFF) + 0x1F4;
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x38 && (g_FieldFxCurrentAge & 1)) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM089SpawnChannel)->pool);
            if (child) {
                child->phase = 1;
                child->x = (rand() & 0x7F) + 0x200;
                child->y = rand();
                child->z = 0;
            }
        }
        if (g_FieldFxCurrentAge >= 0x40) return 1;
        if (FieldFx_IsPlayerWithinRadius(state, 0xF0) == 0) break;
        if (g_FieldFxCurrentAge >= 0x28) break;
        if (FX_TASK_AS(RoomM089PulseEvent)->active) {
            channel = ANIM_OWNER_AS(RoomM089PulseChannel);
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                ((RoomM089PulseEntity *)D_8009CD10)->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x29000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        break;
    case 2:
        fade = rcos(g_FieldFxCurrentAge << 5) / 32;
        outer = rcos(g_FieldFxCurrentAge << 5) + 0x400;
        tall = rsin(g_FieldFxCurrentAge << 5) + 0x400;
        glow = rcos(g_FieldFxCurrentAge << 5) / 32;
        wide = rsin(g_FieldFxCurrentAge << 5) + 0x800;
        low = 0x800 - rsin(g_FieldFxCurrentAge << 5) / 4;
        g_FieldFxSpriteParams.depth = 0x10;
        outer /= 4;
        wide /= 3;
        if (g_FieldFxCurrentAge < 0x21) {
            u16 *index;
            rotation.x = 0;
            rotation.y = -(g_FieldFxCurrentAge << 5);
            rotation.z = 0;
            index = &g_FieldFxTpageSlot11;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 1, 0, 0));
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 6
                                                                     : palette + 2));
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = state->position.x;
            matrix.t[1] = state->position.y;
            matrix.t[2] = state->position.z;
            scale.x = outer;
            scale.y = tall;
            scale.z = outer;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_RoomM089PulseModel);
            FieldFx_ModelScaleColors(g_RoomM089PulseModel, fade);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089PulseModel, &matrix);
            FieldFx_ModelRestoreColors(g_RoomM089PulseModel);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = g_FieldFxCurrentAge << 5;
            RotMatrixYXZ(&rotation, &wideMatrix);
            wideMatrix.t[0] = state->position.x;
            wideMatrix.t[1] = state->position.y;
            wideMatrix.t[2] = state->position.z;
            wideScale.x = wide;
            wideScale.y = low;
            wideScale.z = wide;
            ScaleMatrix(&wideMatrix, &wideScale);
            FieldFx_ModelSaveColors(g_RoomM089PulseModel);
            FieldFx_ModelScaleColors(g_RoomM089PulseModel, glow);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089PulseModel, &wideMatrix);
            FieldFx_ModelRestoreColors(g_RoomM089PulseModel);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = g_FieldFxCurrentAge << 4;
            RotMatrixYXZ(&rotation, &lowMatrix);
            lowMatrix.t[0] = state->position.x;
            lowMatrix.t[1] = state->position.y;
            lowMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = low * 3 / 2;
            lowScale.z = outer;
            ScaleMatrix(&lowMatrix, &lowScale);
            FieldFx_ModelSaveColors(g_RoomM089PulseModel);
            FieldFx_ModelScaleColors(g_RoomM089PulseModel, glow);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089PulseModel, &lowMatrix);
            FieldFx_ModelRestoreColors(g_RoomM089PulseModel);
        }
        if (g_FieldFxCurrentAge < 0x19) {
            u16 *index;
            rotation.x = 0;
            rotation.y = g_FieldFxCurrentAge << 6;
            rotation.z = 0;
            fade = rcos((g_FieldFxCurrentAge << 10) / 24) / 32;
            outer = rsin((g_FieldFxCurrentAge << 10) / 24) * 3 / 2;
            index = &g_FieldFxTpageSlot11;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 3, 0, 0));
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 7
                                                                     : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &coreMatrix);
            coreMatrix.t[0] = state->position.x;
            coreMatrix.t[1] = state->position.y;
            coreMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = outer;
            lowScale.z = outer;
            ScaleMatrix(&coreMatrix, &lowScale);
            FieldFx_ModelSaveColors(g_RoomM089CoreModel);
            FieldFx_ModelScaleColors(g_RoomM089CoreModel, fade);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM089CoreModel, &coreMatrix);
            FieldFx_ModelRestoreColors(g_RoomM089CoreModel);
        }
        if (g_FieldFxCurrentAge < 0x31) {
            FieldFx_SampleColorTrack(g_RoomM089PulseColorTrack, &color, g_FieldFxCurrentAge);
            FieldFx_DrawStarFan(&state->position, 0x1F4, 0x1F4, 0xC, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        break;
    }
    return 0;
}

short *RoomM089_SetAnchor(int arg0, short arg1, short arg2, short arg3) {
    if (arg0 == 1) {
        g_RoomM089Anchor[0] = arg1;
        g_RoomM089Anchor[1] = arg2;
        g_RoomM089Anchor[2] = arg3;
    }

    return g_RoomM089Anchor;
}
