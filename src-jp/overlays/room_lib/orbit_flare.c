/*
 * The orbit flare: a controller that rises from the scene anchor while
 * shedding drift particles, then orbits the anchor until it leaves the
 * walkable area or touches the floor, flashing the scene.
 *
 * Twenty room and scene overlays link the same two functions in this order,
 * with the controller's two rotation seeds as the only read-only data; this
 * unit is that object, compiled into each of them.
 */

#include "pe1/field_texture_tables.h"
#include "pe1/room_flare.h"

static const GteRotation s_OrbitFlareRotation = { 0, 0, -60, 0 };
static const GteRotation s_OrbitFlareSpin = { 0, 0, 0, 0 };

/* Drift particle shed by the controller: mode 1 moves it up with a random
 * sideways jitter for fourteen frames, mode 2 draws it fading out. */
int RoomEffect_OrbitFlareDrift(int mode, RoomFxDriftParticle *rec) {
    s32 value;
    s32 scale;
    s32 tex;
    s32 fade;

    if (mode != 1) {
        if (mode == 2) {
            goto mode2;
        }
        return 0;

    }
mode1:
    rec->y -= 2;
    rec->x += (rand() & 7) - 3;
    rec->z += (rand() & 7) - 3;
    if (g_FieldFxCurrentAge >= 0xE) {
        return 1;

    mode2:
        {
            fade = rcos((g_FieldFxCurrentAge << 10) / 14) / 50;
            value = ((rcos((g_FieldFxCurrentAge << 10) / 14) / 2) + 0x800) * rec->radius;
            if (value < 0) {
                value += 0xFFF;
            }
            scale = (value >> 12) + 0x400;
            tex = g_FieldFxPaletteTable[g_FieldFxSpriteParams.palette];
            if (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) {
                tex += 4;
            }
            FieldFx_DrawShapeQuads((GteShortVector *)rec, 0, scale, scale,
                          ((s16)g_FieldFxSpriteParams.parameter02 * ((g_FieldFxCurrentAge << 3) / 14)) + 0xE0,
                          GetClut(0x40, tex), 1, fade, 0);
        }

    }
ret0:
    return 0;
}

/* Flashes the scene: marks the battle actor and rewrites the colour code of
 * the first packet in the primary effect channel. */
#define ROOMEFFECT_ORBIT_FLARE_FLASH()                                       \
    {                                                                        \
        RoomFlareChannel *channel = ANIM_OWNER_AS(RoomFlareChannel);                              \
        RoomFlarePacket *packet;                                             \
        if (((*(RoomFlarePacket **)channel->pool)->code & 0x3F000000) ==     \
            0x01000000) {                                                    \
            ((RoomFlareBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;                              \
            packet = *(RoomFlarePacket **)channel->pool;                     \
            packet->code = (packet->code & 0xC0FFFFFF) | 0x11000000;         \
            packet = *(RoomFlarePacket **)channel->pool;                     \
            packet->code |= 0x80000000;                                      \
        }                                                                    \
    }

/* Controller that rises from the scene anchor for fifty-two frames while
 * shedding drift particles, then orbits the anchor at the spawn radius until
 * it leaves the walkable area or touches the floor, flashing the scene. */
int RoomEffect_OrbitFlareController(int mode, RoomOrbitFlareState *state,
                                    RoomOrbitFlareSpawn *spawn) {
    GteRotation rotation = s_OrbitFlareRotation;
    GteRotation spin = s_OrbitFlareSpin;
    GteShortVector floorPos;
    GteRotation floorSpin;
    GteShortVector floorPos2;
    GteRotation floorSpin2;
    RoomFxDriftParticle *child;
    char *pool;
    int height;
    int scale;
    int fade;

    switch (mode) {
    case 0:
        state->x = ((RoomFlareAnchor *)ANIM_OWNER_AS(RoomFlareChannel)->pool)->x;
        state->y = ((RoomFlareAnchor *)ANIM_OWNER_AS(RoomFlareChannel)->pool)->y;
        state->z = ((RoomFlareAnchor *)ANIM_OWNER_AS(RoomFlareChannel)->pool)->z;
        state->state = 0;
        state->frame = 0;
        state->soundHandle = -1;
        switch (FX_TASK_AS(RoomFlareEventState)->phase) {
        case 0:
            state->angle = -spawn->angle;
            state->soundHandle = FieldFx_PlaySoundAtOwner(0x565, FieldFx_GetOwnerProgress());
            break;
        case 1:
            state->angle = 0;
            break;
        case 2:
            state->angle = spawn->angle;
            break;
        }
        pool = ANIM_POOL_AS(RoomFlareChannel)->pool;
        return FieldFx_InitCallbackList(pool, 12, 16,
                             (FieldFxCallbackListCallback)RoomEffect_OrbitFlareDrift);
    case 1:
        state->frame++;
        switch (state->state) {
        case 0:
            if (spawn->attached) {
                FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomFlareChannel)->pool, 0, &rotation, state);
            } else {
                FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomFlareChannel)->pool, 0x19, &rotation, state);
            }
            height = rsin(((s16)state->frame << 10) / 52) * 3 / 2;
            state->height = height;
            state->spread = (s16)height / 48;
            if (g_FieldFxCurrentAge % 3 == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomFlareChannel)->pool);
                if (child) {
                    child->x = state->x;
                    child->y = state->y;
                    child->z = state->z;
                    child->radius = state->height;
                }
            }
            if ((s16)state->frame < 52) break;
            state->state = 1;
            state->frame = 0;
            FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomFlareChannel)->pool, 0,
                          (GteShortVector *)&spin);
            state->angle = (s16)(state->angle + 0x400) - spin.y;
            if (FX_TASK_AS(RoomFlareEventState)->active) {
                RoomFlareNode **slot = (RoomFlareNode **)ANIM_OWNER_AS(RoomFlareChannel)->pool;
                if (slot && *slot) {
                    u8 *flag = (*slot)->state;
                    if (*flag == 1) *flag = 2;
                }
            }
            break;
        case 1:
            state->x += rcos(state->angle) * spawn->radius / 4096;
            state->z += rsin(state->angle) * spawn->radius / 4096;
            state->height = 0x1000;
            if (g_FieldFxCurrentAge & 1) {
                state->spread = 0x80;
            } else {
                state->spread = 0x60;
            }
            if (g_FieldFxCurrentAge & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomFlareChannel)->pool);
                if (child) {
                    child->x = state->x;
                    child->y = state->y;
                    child->z = state->z;
                    child->radius = state->height;
                }
            }
            if (Geo_PointInPoly(state->x << 16, state->z << 16, g_FloorPolygonVertices,
                              g_FloorPolygonVertexCount) == 0) {
                state->state = 2;
                state->frame = 0;
                if (state->soundHandle != -1) {
                    Akao_Cmd_21(state->soundHandle, 0);
                }
            }
            if (FieldFx_IsPlayerWithinRadius(state, 0x39) == 0) break;
            if (FX_TASK_AS(RoomFlareEventState)->active) {
                ROOMEFFECT_ORBIT_FLARE_FLASH();
            }
            state->state = 2;
            state->frame = 0;
            if (state->soundHandle != -1) {
                Akao_Cmd_21(state->soundHandle, 0);
            }
            break;
        case 2:
            state->y--;
            if ((s16)state->frame < 16) break;
            return 1;
        }
        break;
    case 2:
    {
        int tpage;
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = tpage;
        switch (state->state) {
        case 0:
        case 1:
            {
            u16 clut;
            int kind;
            int palette;
            spin.z = g_FieldFxCurrentAge << 7;
            /* Palette read-back: cse folds the kind to the palette just
             * stored, and the read keeps the block address in a register. */
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)state, &spin, state->height, state->height,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xDC, clut, 1,
                          state->spread, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)state, 0, state->height * 2, state->height * 2,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xD8, clut, 3,
                          state->spread, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            floorSpin.x = 0x400;
            floorSpin.y = 0;
            floorSpin.z = 0;
            floorSpin.flags = 1;
            floorPos.x = state->x;
            floorPos.z = state->z;
            floorPos.y = g_RoomFloorY->y;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads(&floorPos, &floorSpin, state->height, state->height,
                          (s16)g_FieldFxSpriteParams.parameter02 * 2 + 0xD8, clut, 3,
                          state->spread, 0);
            }
            break;
        case 2:
            {
            u16 clut;
            int kind;
            int palette;
            scale = rsin((s16)state->frame << 6) + 0x800;
            fade = rcos((s16)state->frame << 6) / 32;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)state, 0, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 + 0xFD, clut, 1, fade, 0);
            }
            {
            u16 clut;
            int kind;
            int palette;
            floorSpin2.x = 0x400;
            floorSpin2.y = 0;
            floorSpin2.z = 0;
            floorSpin2.flags = 1;
            floorPos2.x = state->x;
            floorPos2.z = state->z;
            floorPos2.y = g_RoomFloorY->y;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x40, palette);
            FieldFx_DrawShapeQuads(&floorPos2, &floorSpin2, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 + 0xFD, clut, 3, fade, 0);
            }
            break;
        }
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot2)];
        g_FieldFxSpriteParams.palette = 2;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.tpage = tpage;
        break;
    }
    }
    return 0;
}
