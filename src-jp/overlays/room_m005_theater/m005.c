#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/room_m005.h"
#include "pe1/room_m005_beacon.h"
#include "pe1/room_script_record.h"
#include "pe1/room_sound_slot.h"

#include "../room_lib/room_lib.h"
#include "RoomM005_FxOrbiter_externs.h"

typedef struct RoomM005_OrbRec {
    short h0;                     /* 0x00: x */
    short h2;                     /* 0x02: heading */
    short h4;                     /* 0x04: z */
    short h6;                     /* 0x06: size, ramps to 0x88 */
    short h8;                     /* 0x08: orbit phase */
    short hA;                     /* 0x0A: radius */
    short hC;                     /* 0x0C: state */
    short hE;                     /* 0x0E: fade counter */
} RoomM005_OrbRec;

typedef struct RoomM005_Dialog {
    short h0;
    short h2;
    unsigned short h4;            /* 0x04: anchor x */
    unsigned short h6;            /* 0x06: anchor y */
    unsigned short h8;            /* 0x08: anchor z */
    short hA;
    short hC[1];                  /* 0x0C: fade target block */
} RoomM005_Dialog;

/* Dialog anchor room_m005 keeps in its scene script record. */
#define RoomM005_ScriptDialog (*(RoomM005_Dialog *)&g_RoomScriptRecord)

int RoomM005_FxOrbiter(int mode, RoomM005_OrbRec *r) {
    unsigned short anchor[3];
    short q[4];
    char out[8];

    if (mode != 1) {
        if (mode == 2) goto burst;
        return 0;

    }
tick:
    if (r->hC != 0) {
        if (r->hC == mode) goto fade;
        return 0;

    }
init:
    anchor[0] = RoomM005_ScriptDialog.h4;
    anchor[1] = RoomM005_ScriptDialog.h6;
    anchor[2] = RoomM005_ScriptDialog.h8;
    r->h2 += rsin(g_FieldFxCurrentAge << 6) / 512;
    r->h0 = anchor[0] + rcos(r->h8) * r->hA / 4096;
    r->h4 = anchor[2] + rsin(r->h8) * r->hA / 4096;
    r->h8 += 0x18;
    if (r->h6 < 0x81) {
        r->h6 += 8;
    }
    if (FX_TASK_AS(RoomDlgState)->h12 == mode) {
        r->hC = 1;
        r->hE = 0;
    }
    return 0;

fade:
    {
        short n = r->hE + 1;
        r->hE = n;
        LoadAverageShort12(r, RoomM005_ScriptDialog.hC, 0x1000 - (n << 7), n << 7, r);
    }
    if (r->hE >= 0x20) {
        return 1;

    burst:
        q[0] = 0;
        q[1] = 0;
        q[2] = r->h8;
        q[3] = 0;
        FieldFx_SampleColorTrack(RoomM005_Rec2, out, (r->h2 + g_FieldFxCurrentAge) & 0x3F);
        {
            int a = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
            if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
                a += 4;
            }
            FieldFx_DrawShapeQuads((GteShortVector *)r, (GteRotation *)q, 0x800, 0x800, (s16)g_FieldFxSpriteParams.parameter02 + 0xD8,
                          GetClut(0x20, a), 3, r->h6, (RenderColor *)out);
        }
        {
            int a = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
            if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
                a += 4;
            }
            FieldFx_DrawShapeQuads((GteShortVector *)r, (GteRotation *)q, 0x1000, 0x1000, ((s16)g_FieldFxSpriteParams.parameter02 << 1) + 0xD8,
                          GetClut(0x20, a), 3, r->h6, (RenderColor *)out);
        }
    }
ret0:
    return 0;
}

#undef RoomM005_ScriptDialog

int RoomM005_Orbiter(int mode, RoomM005OrbiterState *state) {
    RoomM005Seed8 seed = g_RoomM005OrbiterJointAnchor;
    s16 target[4];
    char *pool;
    RoomM005OrbiterChild *child;
    RoomSoundSlot *soundSlot;
    int handle;
    int spread;
    int volume;

    switch (mode) {
    case 0:
        state->armed = 0;
        state->frame = 0;
        pool = ANIM_POOL_AS(RoomM005Channel)->pool;
        return FieldFx_InitCallbackList(pool, 16, 24,
                             (FieldFxCallbackListCallback)RoomM005_FxOrbiter);
    case 1:
        if (g_FieldFxCurrentAge == 7) {
            soundSlot = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
            if (soundSlot->channel != 0) {
                volume = 0x7F;
                handle = FieldFx_GetOwnerProgress();
                Akao_SendTableCommand(soundSlot->channel, 0x5AB, handle, 0x80, volume);
                if (soundSlot->channel != 0) {
                    Akao_SendTableCommand(soundSlot->channel, 0x5AC, 0x80, 0x80, volume);
                }
            }
        }
        if (g_FieldFxCurrentAge < 25) {
            pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
            FieldFx_GetActorPosition(pool, 0, target);
            pool = ANIM_POOL_AS(RoomM005Channel)->pool;
            child = FieldFx_AcquireCallbackSlot(pool);
            if (child) {
                child->seed = rand();
                spread = rand() & 0x1FF;
                child->y = target[1] + spread - 0x100;
                child->flag = 0;
                child->radius = (rand() & 0xFF) + 0x100;
                child->state = 0;
                child->timer = 0;
            }
        }
        if (state->armed == 1) {
            state->frame++;
            if ((s16)state->frame >= 32) return 1;
        } else if (FX_TASK_AS(RoomM005EventState)->triggered != 0) {
            state->armed = 1;
        }
        break;
    case 2:
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        FieldFx_GetActorPosition(pool, 0, (s16 *)g_RoomM005ScriptAnchorView);
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        FieldFx_TransformMatrixPoint(pool, 20, &seed, g_RoomM005ScriptFadeView);
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
        }
        break;
    }
    return 0;
}

/* Cross flare: two small sprites either side of joint 9 while the frame
 * counter is below 35, then a flash track, two streaks and a large turning
 * sprite at joint 20. The first draw's intensity, the streak step and its
 * width are separate variables (all land in s0), while the streak fade and
 * the final size share one, and parameter02 is written second: each of
 * these decides retail's register choice. */

int RoomM005_CrossFlare(int mode) {
    GteShortVector position;
    GteRotation rotation;
    RoomM005Seed8 outer = g_RoomM005CrossFlareFlashAnchor;
    RoomM005Seed8 inner = g_RoomM005CrossFlareTwinAnchor;
    RenderColor color;
    RoomSoundSlot *soundSlot;
    int volume;
    int handle;
    int size;
    int step;
    int width;
    int level;

    switch (mode) {
    case 0:
        soundSlot = (RoomSoundSlot *)g_LoadedSceneAssetBlock;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            handle = FieldFx_GetOwnerProgress();
            Akao_SendTableCommand(soundSlot->channel, 0x5F8, handle, 0x80, volume);
            if (soundSlot->channel != 0) {
                Akao_SendTableCommand(soundSlot->channel, 0x5F9, 0x80, 0x80, volume);
            }
        }
        break;
    case 1:
        if (g_FieldFxCurrentAge >= 35) return 1;
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 0x40;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005Channel)->pool, 9, &inner, &position);
        if (g_FieldFxCurrentAge < 35) {
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 6;
            rotation.flags = 0;
            level = rsin((g_FieldFxCurrentAge << 11) / 34);
            position.x -= 0x10;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&position, &rotation, 0x1000, 0x1000, 2,
                              GetClut(0x10, palette), 1, level, 0);
            }
            position.x += 0x20;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&position, &rotation, 0x1000, 0x1000, 2,
                              GetClut(0x10, palette), 1, level, 0);
            }
        }
        g_FieldFxSpriteParams.depth = 0x18;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005Channel)->pool, 0x14, &outer, &position);
        if (g_FieldFxCurrentAge < 35) {
            step = g_FieldFxCurrentAge - 18;
            if ((unsigned int)step < 17) {
                size = rcos(step << 6);
                width = 600 - step * 100;
                FieldFx_SampleColorTrack(g_RoomM005CrossFlareColorTrack, &color, step * 2);
                FieldFx_DrawRingBand(&position, width, 700, 0x18, 0, size, size,
                              0, &color, 0x40, 1);
                FieldFx_DrawRingBand(&position, 700, 800, 0x18, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            FieldFx_SampleColorTrack(g_RoomM005CrossFlareColorTrack, &color, g_FieldFxCurrentAge);
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            size = rsin((g_FieldFxCurrentAge << 10) / 34);
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 6;
            rotation.flags = 0;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&position, &rotation, size, size, 0x40,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 6 : palette + 2),
                              1, 0x60, &color);
            }
        }
        break;
    }
    return 0;
}

/* Sprite that drifts by a fixed velocity for six frames, then draws itself
 * at a room offset with a cosine-driven intensity. */
int RoomM005_BeaconDriftCallback(int mode, RoomM005DriftingSpriteState *state) {
    GteShortVector position;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    int v;
    u16 clut;

    color = g_RoomM005BeaconTint;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        if (g_FieldFxCurrentAge >= 6) return 1;
        break;
    case 2:
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        position.x += g_RoomM005BeaconOffset.x;
        position.y += g_RoomM005BeaconOffset.y;
        position.z += g_RoomM005BeaconOffset.z;
        u = (g_FieldFxCurrentAge & 1) << 7;
        v = ((g_FieldFxCurrentAge << 3) & 0x10) + 0x20;
        scale = rcos((g_FieldFxCurrentAge << 10) / 6) / 24;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0, palette);
        FieldFx_DrawRotatedQuad(&g_RoomM005BeaconCenter, (GteRotation *)&position, 0x44C, 0x6E, u, v,
                      0x7F, 0xF, clut, &color, &color, (short)scale, 1);
        break;
    }
    return 0;
}

/* Joint beacon: rides joint 0x14, sheds drifting sprites for the first
 * frames, swells and shrinks, tags the player on contact; draws four passes
 * of a model, three glow sprites and a turning second model. */
int RoomM005_JointBeacon(int mode, RoomM005Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM005BeaconOffset offset = g_RoomM005JointBeaconAnchor;
    GteShortVector spinAngles;
    GteMatrix matrix;
    GteMatrix spin;
    GteVector spinScale;
    RoomM005BeaconSprite *sprite;
    int size;
    int alpha;
    int page;

    switch (mode) {
    case 0:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005BeaconChannel)->pool, 0x14, &offset, state);
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM005BeaconChannel)->pool, 0,
                      &state->rotation);
        state->rotation.pad = 1;
        state->phase = 0;
        state->timer = 0;
        state->height = 0;
        if (FX_TASK_AS(RoomM005BeaconEvent)->active) {
            RoomM005BeaconPool *pool = ANIM_OWNER_AS(RoomM005BeaconChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        g_RoomM005BeaconModel0 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5462704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM005BeaconModel0, 0, 0);
        g_RoomM005BeaconModel1 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5862704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM005BeaconModel1, 0, 0);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM005BeaconSpawnChannel)->pool, 0x10, 8,
                             (FieldFxCallbackListCallback)RoomM005_BeaconDriftCallback);
    case 1:
        if (g_FieldFxCurrentAge < 10) {
            sprite = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM005BeaconSpawnChannel)->pool);
            if (sprite) {
                sprite->x = (rand() & 0x3FF) - 0x200;
                sprite->y = (rand() & 0x3FF) - 0x200;
                sprite->z = 0;
                sprite->vx = (rand() & 7) - 3;
                sprite->vy = (rand() & 7) - 3;
                sprite->vz = 0;
            }
        }
        switch (state->phase) {
        case 0:
            state->timer++;
            state->width = 0x1000;
            state->height += 0x200;
            if (state->timer >= 5) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->width = 0x1000 - (state->timer << 9);
            if (state->timer >= 8) return 1;
            break;
        }
        FieldFx_AnglesToVector(&state->rotation, state->height, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (FieldFx_IsPlayerInSegmentBand(state, &state->target, 100) == 0) return 0;
        if (FX_TASK_AS(RoomM005BeaconEvent)->active) {
            RoomM005BeaconChannel *channel = ANIM_OWNER_AS(RoomM005BeaconChannel);
            if ((channel->pool->node->flags & 0x3F000000) != 0x01000000) return 0;
            ((RoomM005BeaconEntity *)D_8009CD10)->actor->flags |= 0x4000;
            channel->pool->node->flags =
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
            channel->pool->node->flags |= 0x80000000;
        }
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 8;
        matrix = *ANIM_OWNER_AS(RoomM005BeaconChannel)->pool->matrix;
        scale.x = scale.y = state->width / 40;
        scale.z = (state->height << 12) / 8680;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RenderMatrixSlot *slot;
            u16 *index = &g_FieldFxTpageSlot3;
            u16 *tpages = g_FieldFxTpageTable;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            slot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
            page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x30, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(g_RoomM005BeaconModel0);
        FieldFx_ModelScaleColors(g_RoomM005BeaconModel0, 0x32);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM005BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM005BeaconModel0);
        FieldFx_ModelSaveColors(g_RoomM005BeaconModel0);
        scale.z = 0x1000;
        scale.x = scale.y = 0x2000;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelShiftColors(g_RoomM005BeaconModel0, -0xFF, -0x82, 0x30);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM005BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM005BeaconModel0);
        FieldFx_ModelSaveColors(g_RoomM005BeaconModel0);
        scale.x = scale.y = 0x400;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelScaleColors(g_RoomM005BeaconModel0, 0xC8);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM005BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM005BeaconModel0);
        FieldFx_ModelSaveColors(g_RoomM005BeaconModel0);
        matrix.t[1] = g_RoomFloorY->y;
        scale.x = scale.y = 0x2000;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelScaleColors(g_RoomM005BeaconModel0, 0x18);
        FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM005BeaconModel0, &matrix);
        FieldFx_ModelRestoreColors(g_RoomM005BeaconModel0);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 1;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0x20;
        alpha = 0xA0;
        size = state->width;
        if (g_FieldFxCurrentAge & 1) {
            alpha = 0x80;
            size = size * 3 / 2;
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&state->position, 0, size, size, 2,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&state->target, 0, state->width / 2, state->width / 2, 2,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y = g_RoomFloorY->y;
        rotation.y = 0;
        rotation.z = 0;
        rotation.x = 0x400;
        rotation.pad = 1;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&position, (GteRotation *)&rotation, size, size, 2,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 8 : palette + 4),
                          3, alpha, 0);
        }
        if (g_FieldFxCurrentAge < 0xD) {
            alpha = rcos((g_FieldFxCurrentAge << 10) / 12) / 32;
            size = rsin((g_FieldFxCurrentAge << 10) / 12);
            {
                RenderMatrixSlot *slot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            {
                u16 *index = &g_FieldFxTpageSlot11;
                g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 1, 0, 0));
            }
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag) ? palette + 7
                                                                     : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            spinAngles.x = state->rotation.x;
            spinAngles.y = state->rotation.y;
            spinAngles.z = state->rotation.z;
            spinAngles.x += 0x800;
            spinAngles.z = g_FieldFxCurrentAge << 7;
            RotMatrixYXZ(&spinAngles, &spin);
            spin.t[0] = state->position.x;
            spin.t[1] = state->position.y;
            spin.t[2] = state->position.z;
            spinScale.z = spinScale.y = spinScale.x = size / 2;
            ScaleMatrix(&spin, &spinScale);
            FieldFx_ModelSaveColors(g_RoomM005BeaconModel1);
            FieldFx_ModelScaleColors(g_RoomM005BeaconModel1, alpha / 2);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM005BeaconModel1, &spin);
            FieldFx_ModelRestoreColors(g_RoomM005BeaconModel1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        g_RoomM005BeaconCenter.x = state->position.x;
        g_RoomM005BeaconCenter.y = state->position.y;
        g_RoomM005BeaconCenter.z = state->position.z;
        g_RoomM005BeaconOffset.x = state->rotation.x;
        g_RoomM005BeaconOffset.y = state->rotation.y;
        g_RoomM005BeaconOffset.z = 0;
        g_RoomM005BeaconOffset.x = 0;
        break;
    }
    return 0;
}
