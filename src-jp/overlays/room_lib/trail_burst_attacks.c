/*
 * Trail and burst attacks: two seeking trails that chase the player, each
 * shedding sparks and flashes, and two model bursts that grow a scaled model
 * at one of the actor's joints while shedding pulsing sprites.
 *
 * room_m245 and room_m397 link these eight functions and the two trail
 * classes' history getters in this order, with the same 0x18 bytes of
 * read-only seeds; this unit is that object, compiled into each of them. The second trail takes its history slot from the event
 * state and scatters its heading with a fixed spread; the second burst
 * follows another joint. The trail histories, colour ramps, model assets,
 * layer records and anchors live in each room's own data.
 */

#include "pe1/gte.h"
#include "pe1/room_trail_burst.h"

static const RenderColor s_TrailHeadColor = { 0xA0, 0xA0, 0xA0, 0 };
static const RenderColor s_TrailTailColor = { 0x00, 0x00, 0x20, 0 };
static const GteRotation s_JointOffset = { 0, 0, -250, 0 };
static const RenderColor s_PulseColor = { 0x80, 0x80, 0x8C, 0 };
static const RenderColor s_BurstShade = { 0x00, 0x00, 0xC8, 0 };

/* First object of the primary channel's pool: its packet word. */
typedef struct RoomSeekingTrailPacket {
    u32 code;
} RoomSeekingTrailPacket;

/* Seeking trail: state 1 is the head that turns towards the player and
 * records its path, state 2 a spark shed from the path and state 3 a flash
 * at the head. A trail point near the player flags the battle actor. */
int RoomEffect_SeekingTrailParticle(int mode, RoomSeekingTrail *trail) {
    RenderColor colorA;
    RenderColor colorB;
    GteRotation rotation;
    GteShortVector target;
    GteShortVector angles;
    GteShortVector unused;
    RoomSeekingTrail *child;
    GteShortVector *point;
    int range;
    int i;
    int scale;
    int kind;
    int palette;
    u16 clut;
    u16 *palettes;

    colorA = s_TrailHeadColor;
    colorB = s_TrailTailColor;
    rotation = s_JointOffset;
    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer = 0;
            trail->state = 1;
            trail->fade = 0;
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 7, &rotation, trail);
            FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, &angles);
            angles.x = 0;
            trail->ax = angles.x;
            trail->ay = angles.y;
            trail->az = angles.z;
            FieldFx_UpdatePointHistory(&g_RoomSeekingTrailHistory[trail->slot], 0x18, trail, 1);
            trail->speed = 0x20;
            trail->turn = 0x200;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (!child) return 0;
            child->x = trail->x;
            child->y = trail->y;
            child->z = trail->z;
            child->state = 3;
            child->timer = 0;
            break;
        case 1:
            trail->timer++;
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            FieldFx_GetActorPosition((char *)D_8009CD10, 1, &target.x);
            FieldFx_CalculateLookAngles(trail, &target, &angles);
            angles.x = 0;
            if (trail->fade == 0) {
                FieldFx_EaseAngles((unsigned short *)&angles, &trail->ax, trail->turn);
                if (trail->timer >= 3) {
                    range = trail->timer * 16 + 0x100;
                    if (!(rand() & 1)) {
                        trail->ay += rand() % (range * 2) - range;
                    } else {
                        trail->ay += rand() % range - (range >> 1);
                    }
                }
                if (trail->speed < 0x3C) trail->speed += 2;
                FieldFx_AnglesToVector((GteShortVector *)&trail->ax, (s16)(trail->speed * trail->speed / 16), &target);
                trail->x += target.x;
                trail->y += target.y;
                trail->z += target.z;
            } else if (++trail->fade >= 16) {
                return 1;
            }
            FieldFx_UpdatePointHistory(point, 0x18, trail, 0);
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            for (i = 0; i < 0x18; i++, point++) {
                if (FieldFx_IsPlayerWithinRadius(point, 0x68) && FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if (((*(RoomSeekingTrailPacket **)channel->pool)->code & 0x3F000000) ==
                        0x01000000) {
                        ((RoomSparkBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x3F000000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x80000000;
                    }
                }
            }
            point = g_RoomSeekingTrailHistory[trail->slot].point;
            if (trail->fade == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    point += (rand() & 3) * 4;
                    child->x = point->x;
                    child->y = point->y;
                    child->z = point->z;
                    child->x += -0x20 + (rand() & 0x3F);
                    child->y += -0x20 + (rand() & 0x3F);
                    child->z += -0x20 + (rand() & 0x3F);
                    child->ax = trail->ax;
                    child->ay = trail->ay;
                    child->az = trail->az;
                    child->ax += -100 + rand() % 200;
                    child->ay += -100 + rand() % 200;
                    child->speed = (rand() & 0x3F) - 0x20;
                    child->turn = (rand() & 0x3F) - 0x20;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (g_FieldFxCurrentAge < 0x20) return 0;
            if (trail->fade != 0) return 0;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = trail->x;
                child->y = trail->y;
                child->z = trail->z;
                child->state = 3;
                child->timer = 0;
            }
            trail->fade = 1;
            break;
        case 2:
            trail->timer++;
            trail->ax += trail->turn;
            trail->ay += trail->speed;
            if (trail->timer >= 20) return 1;
            break;
        case 3:
            if (++trail->timer < 24) break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (trail->state) {
        case 1:
            scale = ((g_FieldFxCurrentAge / 3) & 1) * 32 + 32;
            i = 0;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            {
                u16 kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            }
            clut = GetClut(0x10, palette);
            FieldFx_DrawTexturedRibbon((GteShortVector *)&g_RoomSeekingTrailHistory[trail->slot], 0x18, 0x78, i, scale,
                          0xFD, 0x1F, g_FieldFxTpageTable[g_FieldFxTpageSlot3], clut, 0x80,
                          &colorA, &colorB, 1);
            FieldFx_DrawStarFan(&g_RoomSeekingTrailHistory[trail->slot].point[rand() & 0xF],
                          0x190, 0x190, 6, 0, 0x1000, 0x1000, &colorA, 0, 0x80, 3);
            return 0;
        case 2:
            scale = rcos((trail->timer << 10) / 20) / 32;
            rcos((trail->timer << 10) / 20);
            /* Shares the loop index's register with retail (s0). */
            i = (g_FieldFxCurrentAge & 1) << 7;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            /* tpage is the first parameter store, so its address stays in a
             * register and parameter00 is later written through it. */
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x10, palette);
            FieldFx_DrawRotatedQuad((GteShortVector *)trail, (GteRotation *)&trail->ax, 0x1FE, 0x78,
                          i, 0, 0x80, 0x20, clut, &colorB, &colorA, (s16)scale, 1);
            palettes = g_FieldFxPaletteTable;
            if (trail->timer < 13) {
                /* Block-scope temporaries: the kind read stays below the
                 * parameter00 store, as in retail. */
                int kind;
                int palette;

                scale = rcos((trail->timer << 10) / 12) / 64;
                g_FieldFxSpriteParams.parameter00 = 0x20;
                g_FieldFxSpriteParams.parameter02 = 2;
                g_FieldFxSpriteParams.extent_x = 0x20;
                g_FieldFxSpriteParams.extent_y = 0x20;
                kind = g_FieldFxSpriteParams.palette;
                palette = palettes[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x40, palette);
                FieldFx_DrawShapeQuads((GteShortVector *)trail, 0, 0x1000, 0x1000,
                              (s16)g_FieldFxSpriteParams.parameter02 * (trail->timer * 2 / 3) + 0x90, clut, 1,
                              scale, 0);
            }
            break;
        case 3:
            scale = rcos((trail->timer << 10) / 24) / 32;
            rcos((trail->timer << 10) / 24);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            target.x = trail->x;
            target.y = trail->y;
            target.z = trail->z;
            target.x += -0x10 + (rand() & 0x1F);
            target.y += -0x10 + (rand() & 0x1F);
            target.z += -0x10 + (rand() & 0x1F);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads(&target, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (trail->timer & 7) + 0x90, clut, 1, scale, 0);
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

/* Trail controller: mode 0 opens a pool of 32 trails, mode 1 launches one
 * on the first frame, into the history slot the caller gives, and finishes
 * after eight frames; mode 2 sets the sprite parameters. */
int RoomEffect_SeekingTrailController(int mode, u16 *slot) {
    RoomSeekingTrail *trail;
    RoomSparkEventState *event;
    u16 tpage;

    switch (mode) {
    case 0:
        event = FX_TASK_AS(RoomSparkEventState);
        *slot = 0;
        if (event->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (nodes != 0 && *nodes != 0) {
                u8 *state = (*nodes)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x18, 0x20,
                             (FieldFxCallbackListCallback)RoomEffect_SeekingTrailParticle);
    case 1:
        if (g_FieldFxCurrentAge == mode) {
            trail = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (trail != 0) {
                trail->slot = *slot;
                trail->state = 0;
                trail->timer = 0;
            }
        }
        if (g_FieldFxCurrentAge < 8) {
            break;
        }
        return 2;
    case 2:
        tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = tpage;
        break;
    }
    return 0;
}

/* Sprite shed by the model burst. Mode 1 drifts it for sixteen frames;
 * mode 2 draws a textured rectangle and a scaled sprite at the burst's
 * anchor, pulsing with a cosine of its frame counter. */
int RoomEffect_ModelBurstParticle(int mode, RoomModelBurstParticle *state) {
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    u16 clut;

    color = s_PulseColor;
    switch (mode) {
    case 1:
        state->frame++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += 0x18;
        if ((short)state->frame >= 16) return 1;
        break;
    case 2:
        scale = rcos((short)state->frame << 6) / 32;
        rcos((short)state->frame << 6);
        u = (g_FieldFxCurrentAge & 1) << 7;
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x10, palette);
        FieldFx_DrawRotatedQuad(&g_RoomModelBurstAnchor, (GteRotation *)state, 0x1FE, 0x6E, u, 0,
                      0x80, 0x20, clut, &color, &color, (short)scale, 1);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x40, palette);
        FieldFx_DrawShapeQuads(&g_RoomModelBurstAnchor, 0, 0x1000, 0x1000,
                      (s16)g_FieldFxSpriteParams.parameter02 * ((short)state->frame / 2) + 0x90, clut, 1, scale / 2, 0);
        break;
    }
    return 0;
}

/* Model burst: loads the model asset, then for 48 frames brightens and sheds
 * one sprite per frame, while drawing a growing sprite pair at joint 7 and
 * the spinning, scaled model coloured from the room's ramp. */
int RoomEffect_ModelBurstController(int mode, s16 *state) {
    GteRotation rotation = s_JointOffset;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RenderColor shade = s_BurstShade;
    GteMatrix matrix;
    GteVector scale;
    RoomModelBurstParticle *child;
    u16 *tpages;
    int brightness;
    int size;
    int handle;

    switch (mode) {
    case 0:
        *state = 0;
        g_RoomModelBurstAsset = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC54A5704);
        FieldFx_ModelScrollUv(g_RoomModelBurstAsset, 0, 0);
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_LoadRotatedClutRow(3, -1, g_RoomModelBurstLayer);
        handle = FieldFx_GetOwnerProgress();
        FieldFx_PlaySoundAtOwner(0x5A4, handle);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x10, 0x10,
                             (FieldFxCallbackListCallback)RoomEffect_ModelBurstParticle);
    case 1:
        if (*state < 0x80) *state += 8;
        if (g_FieldFxCurrentAge >= 2) {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
            FieldFx_LoadRotatedClutRow(3, g_FieldFxCurrentAge * 2, g_RoomModelBurstLayer);
        }
        child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
        if (child) {
            child->x = rand();
            child->y = rand();
            child->z = rand();
            child->frame = 0;
            child->reserved08 = 0;
            child->vx = (rand() & 0x7F) - 0x40;
            child->vy = (rand() & 0x7F) - 0x40;
        }
        if (g_FieldFxCurrentAge < 0x30) break;
        return 1;
    case 2:
        {
            int tpage;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x18;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 7, &rotation, &position);
        spin.x = rsin(g_FieldFxCurrentAge * 80) / 4;
        spin.y = rcos(g_FieldFxCurrentAge << 7) / 8;
        spin.flags = 0;
        spin.z = -g_FieldFxCurrentAge << 3;
        brightness = *state;
        if (g_FieldFxCurrentAge != 0) brightness = brightness * 2 / 3;
        size = rsin((g_FieldFxCurrentAge << 10) / 48) / 4 + 0x400;
        FieldFx_SampleColorTrack(g_RoomModelBurstColors, &color, g_FieldFxCurrentAge * 3);
        FieldFx_DrawStarFan(&position, 0x124, 0x124, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness, 1);
        FieldFx_DrawRingBand(&position, 0xC8, 0x1F4, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness / 2, 1);
        g_RoomModelBurstAnchor.x = position.x;
        g_RoomModelBurstAnchor.y = position.y;
        g_RoomModelBurstAnchor.z = position.z;
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_SampleColorTrack(g_RoomModelBurstColors, &color, g_FieldFxCurrentAge);
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
            clut = GetClut(0x30, palette);
            FieldFx_ModelSetTexture(page, clut);
        }
        FieldFx_ModelSetSemiTrans(1);
        RotMatrixYXZ((GteShortVector *)&spin, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelSaveColors(g_RoomModelBurstAsset);
        FieldFx_ModelSetColors(g_RoomModelBurstAsset, color.r, color.g, color.b);
        FieldFx_ModelScaleColors(g_RoomModelBurstAsset, (u16)(brightness / 2));
        FieldFx_ModelDraw(g_RoomModelBurstAsset, &matrix);
        FieldFx_ModelRestoreColors(g_RoomModelBurstAsset);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.depth = 8;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* The second trail: it takes its history slot from the event state, nudges
 * its initial heading, starts from joint 5 and jitters with a fixed spread. */
int RoomEffect_ScatterTrailParticle(int mode, RoomSeekingTrail *trail) {
    RenderColor colorA;
    RenderColor colorB;
    GteRotation rotation;
    GteShortVector target;
    GteShortVector angles;
    RoomSeekingTrail *child;
    GteShortVector *point;
    int i;
    int scale;
    int kind;
    int palette;
    u16 clut;
    u16 *palettes;

    colorA = s_TrailHeadColor;
    colorB = s_TrailTailColor;
    rotation = s_JointOffset;
    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer = 0;
            trail->state = 1;
            trail->slot = FX_TASK_AS(RoomSparkEventState)->trailSlot;
            trail->fade = 0;
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 5, &rotation, trail);
            FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, &angles);
            angles.x = 0;
            angles.y += (rand() & 0x1FF) - 0x100;
            trail->ax = angles.x;
            trail->ay = angles.y;
            trail->az = angles.z;
            FieldFx_UpdatePointHistory(&g_RoomScatterTrailHistory[trail->slot], 0x18, trail, 1);
            trail->speed = 0x20;
            trail->turn = 0x200;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (!child) return 0;
            child->x = trail->x;
            child->y = trail->y;
            child->z = trail->z;
            child->state = 3;
            child->timer = 0;
            break;
        case 1:
            trail->timer++;
            point = g_RoomScatterTrailHistory[trail->slot].point;
            FieldFx_GetActorPosition((char *)D_8009CD10, 1, &target.x);
            FieldFx_CalculateLookAngles(trail, &target, &angles);
            angles.x = 0;
            if (trail->fade == 0) {
                FieldFx_EaseAngles((unsigned short *)&angles, &trail->ax, trail->turn);
                if (!(rand() & 1)) {
                    trail->ay += rand() % 800 - 400;
                } else {
                    trail->ay += (rand() & 0x1FF) - 0x100;
                }
                if (trail->speed < 0x3C) trail->speed += 2;
                FieldFx_AnglesToVector((GteShortVector *)&trail->ax, (s16)(trail->speed * trail->speed / 16), &target);
                trail->x += target.x;
                trail->y += target.y;
                trail->z += target.z;
            } else if (++trail->fade >= 16) {
                return 1;
            }
            FieldFx_UpdatePointHistory(point, 0x18, trail, 0);
            point = g_RoomScatterTrailHistory[trail->slot].point;
            for (i = 0; i < 0x18; i++, point++) {
                if (FieldFx_IsPlayerWithinRadius(point, 0x64) && FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if (((*(RoomSeekingTrailPacket **)channel->pool)->code & 0x3F000000) ==
                        0x01000000) {
                        ((RoomSparkBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x3F000000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x80000000;
                    }
                }
            }
            point = g_RoomScatterTrailHistory[trail->slot].point;
            if (trail->fade == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    point += (rand() & 3) * 4;
                    child->x = point->x;
                    child->y = point->y;
                    child->z = point->z;
                    child->x += -0x20 + (rand() & 0x3F);
                    child->y += -0x20 + (rand() & 0x3F);
                    child->z += -0x20 + (rand() & 0x3F);
                    child->ax = trail->ax;
                    child->ay = trail->ay;
                    child->az = trail->az;
                    child->ax += -100 + rand() % 200;
                    child->ay += -100 + rand() % 200;
                    child->speed = (rand() & 0x3F) - 0x20;
                    child->turn = (rand() & 0x3F) - 0x20;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (g_FieldFxCurrentAge < 0x20) return 0;
            if (trail->fade != 0) return 0;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = trail->x;
                child->y = trail->y;
                child->z = trail->z;
                child->state = 3;
                child->timer = 0;
            }
            trail->fade = 1;
            break;
        case 2:
            trail->timer++;
            trail->ax += trail->turn;
            trail->ay += trail->speed;
            if (trail->timer >= 20) return 1;
            break;
        case 3:
            if (++trail->timer < 24) break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (trail->state) {
        case 1:
            scale = ((g_FieldFxCurrentAge / 2) & 1) * 32 + 32;
            i = 0;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            {
                u16 kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            }
            clut = GetClut(0x10, palette);
            FieldFx_DrawTexturedRibbon((GteShortVector *)&g_RoomScatterTrailHistory[trail->slot], 0x18, 0x78, i, scale,
                          0xFD, 0x1F, g_FieldFxTpageTable[g_FieldFxTpageSlot3], clut, 0x80,
                          &colorA, &colorB, 1);
            FieldFx_DrawStarFan(&g_RoomScatterTrailHistory[trail->slot].point[rand() & 0xF],
                          0x190, 0x190, 6, 0, 0x1000, 0x1000, &colorA, 0, 0x80, 3);
            return 0;
        case 2:
            scale = rcos((trail->timer << 10) / 20) / 32;
            rcos((trail->timer << 10) / 20);
            /* Shares the loop index's register with retail (s0). */
            i = (g_FieldFxCurrentAge & 1) << 7;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            /* tpage is the first parameter store, so its address stays in a
             * register and parameter00 is later written through it. */
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x10, palette);
            FieldFx_DrawRotatedQuad((GteShortVector *)trail, (GteRotation *)&trail->ax, 0x1FE, 0x6E,
                          i, 0, 0x80, 0x20, clut, &colorB, &colorA, (s16)scale, 1);
            palettes = g_FieldFxPaletteTable;
            if (trail->timer < 13) {
                /* Block-scope temporaries: the kind read stays below the
                 * parameter00 store, as in retail. */
                int kind;
                int palette;

                scale = rcos((trail->timer << 10) / 12) / 64;
                g_FieldFxSpriteParams.parameter00 = 0x20;
                g_FieldFxSpriteParams.parameter02 = 2;
                g_FieldFxSpriteParams.extent_x = 0x20;
                g_FieldFxSpriteParams.extent_y = 0x20;
                kind = g_FieldFxSpriteParams.palette;
                palette = palettes[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                clut = GetClut(0x40, palette);
                FieldFx_DrawShapeQuads((GteShortVector *)trail, 0, 0x1000, 0x1000,
                              (s16)g_FieldFxSpriteParams.parameter02 * (trail->timer * 2 / 3) + 0x90, clut, 1,
                              scale, 0);
            }
            break;
        case 3:
            scale = rcos((trail->timer << 10) / 24) / 32;
            rcos((trail->timer << 10) / 24);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            target.x = trail->x;
            target.y = trail->y;
            target.z = trail->z;
            target.x += -0x10 + (rand() & 0x1F);
            target.y += -0x10 + (rand() & 0x1F);
            target.z += -0x10 + (rand() & 0x1F);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads(&target, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (trail->timer & 7) + 0x90, clut, 1, scale, 0);
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

/* Controller of the second trail, as the first. */
int RoomEffect_ScatterTrailController(int mode, u16 *slot) {
    RoomSeekingTrail *trail;
    RoomSparkEventState *event;
    u16 tpage;

    switch (mode) {
    case 0:
        event = FX_TASK_AS(RoomSparkEventState);
        *slot = 0;
        if (event->active != 0) {
            RoomSparkNode **nodes = (RoomSparkNode **)ANIM_OWNER_AS(RoomSparkChannel)->pool;
            if (nodes != 0 && *nodes != 0) {
                u8 *state = (*nodes)->state;
                if (*state == 1) {
                    *state = 2;
                }
            }
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x18, 0x20,
                             (FieldFxCallbackListCallback)RoomEffect_ScatterTrailParticle);
    case 1:
        if (g_FieldFxCurrentAge == mode) {
            trail = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (trail != 0) {
                trail->slot = *slot;
                trail->state = 0;
                trail->timer = 0;
            }
        }
        if (g_FieldFxCurrentAge < 8) {
            break;
        }
        return 2;
    case 2:
        tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = tpage;
        break;
    }
    return 0;
}

/* Sprite shed by the second burst, drawn at its anchor. */
int RoomEffect_ScatterBurstParticle(int mode, RoomModelBurstParticle *state) {
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    u16 clut;

    color = s_PulseColor;
    switch (mode) {
    case 1:
        state->frame++;
        state->x += state->vx;
        state->y += state->vy;
        state->z += 0x18;
        if ((short)state->frame >= 16) return 1;
        break;
    case 2:
        scale = rcos((short)state->frame << 6) / 32;
        rcos((short)state->frame << 6);
        u = (g_FieldFxCurrentAge & 1) << 7;
        matrixSlot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x10, palette);
        FieldFx_DrawRotatedQuad(&g_RoomScatterBurstAnchor, (GteRotation *)state, 0x1FE, 0x64, u, 0,
                      0x80, 0x20, clut, &color, &color, (short)scale, 1);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
        clut = GetClut(0x40, palette);
        FieldFx_DrawShapeQuads(&g_RoomScatterBurstAnchor, 0, 0x1000, 0x1000,
                      (s16)g_FieldFxSpriteParams.parameter02 * ((short)state->frame / 2) + 0x90, clut, 1, scale / 2, 0);
        break;
    }
    return 0;
}

/* The second burst, following joint 5. */
int RoomEffect_ScatterBurstController(int mode, s16 *state) {
    GteRotation rotation = s_JointOffset;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RenderColor shade = s_BurstShade;
    GteMatrix matrix;
    GteVector scale;
    RoomModelBurstParticle *child;
    u16 *tpages;
    int brightness;
    int size;
    int handle;

    switch (mode) {
    case 0:
        *state = 0;
        g_RoomScatterBurstAsset = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC54A5704);
        FieldFx_ModelScrollUv(g_RoomScatterBurstAsset, 0, 0);
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_LoadRotatedClutRow(3, -1, g_RoomScatterBurstLayer);
        handle = FieldFx_GetOwnerProgress();
        FieldFx_PlaySoundAtOwner(0x5A4, handle);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x10, 0x10,
                             (FieldFxCallbackListCallback)RoomEffect_ScatterBurstParticle);
    case 1:
        if (*state < 0x80) *state += 8;
        if (g_FieldFxCurrentAge >= 2) {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.tpage = tpage;
            FieldFx_LoadRotatedClutRow(3, g_FieldFxCurrentAge * 2, g_RoomScatterBurstLayer);
        }
        child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
        if (child) {
            child->x = rand();
            child->y = rand();
            child->z = rand();
            child->frame = 0;
            child->reserved08 = 0;
            child->vx = (rand() & 0x7F) - 0x40;
            child->vy = (rand() & 0x7F) - 0x40;
        }
        if (g_FieldFxCurrentAge < 0x30) break;
        return 1;
    case 2:
        {
            int tpage;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x18;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 5, &rotation, &position);
        spin.x = rsin(g_FieldFxCurrentAge * 80) / 4;
        spin.y = rcos(g_FieldFxCurrentAge << 7) / 8;
        spin.flags = 0;
        spin.z = -g_FieldFxCurrentAge << 3;
        brightness = *state;
        if (g_FieldFxCurrentAge != 0) brightness = brightness * 2 / 3;
        size = rsin((g_FieldFxCurrentAge << 10) / 48) / 4 + 0x400;
        FieldFx_SampleColorTrack(g_RoomScatterBurstColors, &color, g_FieldFxCurrentAge * 3);
        FieldFx_DrawStarFan(&position, 0x124, 0x124, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness, 1);
        FieldFx_DrawRingBand(&position, 0xC8, 0x1F4, 12, 0, 0x1000, 0x1000, &shade, 0,
                      brightness / 2, 1);
        g_RoomScatterBurstAnchor.x = position.x;
        g_RoomScatterBurstAnchor.y = position.y;
        g_RoomScatterBurstAnchor.z = position.z;
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_SampleColorTrack(g_RoomScatterBurstColors, &color, g_FieldFxCurrentAge);
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
            clut = GetClut(0x30, palette);
            FieldFx_ModelSetTexture(page, clut);
        }
        FieldFx_ModelSetSemiTrans(1);
        RotMatrixYXZ((GteShortVector *)&spin, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelSaveColors(g_RoomScatterBurstAsset);
        FieldFx_ModelSetColors(g_RoomScatterBurstAsset, color.r, color.g, color.b);
        FieldFx_ModelScaleColors(g_RoomScatterBurstAsset, (u16)(brightness / 2));
        FieldFx_ModelDraw(g_RoomScatterBurstAsset, &matrix);
        FieldFx_ModelRestoreColors(g_RoomScatterBurstAsset);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.depth = 8;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* History getters of the seeking and the scatter trail classes; retail has
 * both return the seeking trail's histories. */
void *RoomEffect_SeekingTrailHistory(void) {
    return g_RoomSeekingTrailHistory;
}

void *RoomEffect_ScatterTrailHistory(void) {
    return g_RoomSeekingTrailHistory;
}
