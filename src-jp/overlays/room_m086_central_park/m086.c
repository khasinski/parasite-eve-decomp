#include "common.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_params.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/room_bounce_glint.h"
#include "pe1/room_fx.h"
#include "pe1/room_m086_seeker.h"

#include "m086_externs.h"

int RoomM086_SeekerCallback(int mode, RoomM086Seeker *seeker, RoomM086SeekerParams *params) {
    GteShortVector rotation;
    GteShortVector position;
    GteMatrix matrix;
    GteVector scale;
    RoomFloorY *floor;
    RoomM086Channel *channel;
    u16 *index;
    int size = params->size >> 3;
    int fall;
    int bounce;
    int page;

    switch (mode) {
    case 1:
        switch (seeker->state) {
        case 0:
            seeker->timer++;
            FieldFx_AnglesToVector(&seeker->heading, params->distance, &position);
            seeker->position.x += position.x;
            seeker->position.y += position.y;
            seeker->position.z += position.z;
            floor = g_RoomFloorY;
            if (seeker->position.y >= floor->y) {
                seeker->state = 1;
                seeker->timer = 0;
            }
            if (FieldFx_IsPlayerWithinRadius(&seeker->position, size * 41 / 512) == 0) break;
            if (seeker->position.y < floor->y - 0x202) break;
            if (FX_TASK_AS(RoomM086EventState)->active) {
                channel = ANIM_OWNER_AS(RoomM086Channel);
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    ((RoomM086BattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x11000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            seeker->state = 1;
            seeker->timer = 0;
            break;
        case 1:
            seeker->timer++;
            seeker->position.y -= 6;
            if (seeker->timer >= 8) return 1;
            break;
        case 2:
            seeker->timer++;
            seeker->position.x += seeker->heading.x;
            seeker->position.y += seeker->heading.y;
            seeker->position.z += seeker->heading.z;
            seeker->heading.x = seeker->heading.x * 31 / 32;
            seeker->heading.z = seeker->heading.z * 31 / 32;
            fall = (u16)seeker->heading.y + 4;
            seeker->heading.y = fall;
            if (seeker->position.y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                seeker->heading.y = bounce;
            }
            if (seeker->timer >= 0x20) return 1;
            break;
        }
        break;
    case 2:
        switch (seeker->state) {
        case 0:
            rotation.x = seeker->heading.x;
            rotation.y = seeker->heading.y;
            rotation.z = seeker->heading.z;
            rotation.x += 0x800;
            rotation.z = seeker->timer * 256;
            index = &g_FieldFxTpageSlot3;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 0xFF, 0, 0));
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_ModelSetTexture(page, GetClut(0x20, palette));
            }
            FieldFx_ModelSetSemiTrans(0);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = seeker->position.x;
            matrix.t[1] = seeker->position.y;
            matrix.t[2] = seeker->position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_RoomM086SeekerModelB);
            FieldFx_ModelScaleColors(g_RoomM086SeekerModelB, 0x80);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM086SeekerModelB, &matrix);
            FieldFx_ModelRestoreColors(g_RoomM086SeekerModelB);
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            rotation.pad = 1;
            position.x = seeker->position.x;
            position.y = seeker->position.y;
            position.z = seeker->position.z;
            position.y = g_RoomFloorY->y;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&position, (GteRotation *)&rotation, size * 8, size * 8, 0xE8,
                              GetClut(0x50, palette), 2, 0x80, 0);
            }
            break;
        case 1:
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&seeker->position, 0, size * 8, size * 8,
                              (s16)g_FieldFxSpriteParams.parameter02 * seeker->timer + 0xC8,
                              GetClut(0x10, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            rcos(seeker->timer << 5);
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = g_FieldFxCurrentAge << 7;
            rotation.pad = 0;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&seeker->position, (GteRotation *)&rotation, 0x555, 0x555, 0x1A,
                              GetClut(0x10, palette), 1, 0x80, 0);
            }
            break;
        }
        break;
    }
    return 0;
}

/* Seeker controller: anchored on joint 7 of the actor, it drifts and falls
 * (shedding sparks on the first frame), then bursts into a spark ring and a
 * fan of seekers once it reaches the floor band. */
int RoomM086_SeekerController(int mode, RoomM086SeekerController *state, RoomM086ControllerParams *params) {
    GteShortVector rotation;
    GteShortVector position;
    RoomM086Offset anchor = g_RoomM086JointAnchor;
    RoomM086Offset drift = g_RoomM086LaunchVelocity;
    GteMatrix matrix;
    GteVector scale;
    RenderMatrixSlot *slot;
    RoomM086Seeker *child;
    u16 *index;
    int size;
    int page;
    int speed;
    int angle;
    int i;

    switch (mode) {
    case 0:
        slot = &g_ViewMatrixSlot;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM086Channel)->pool, 7, &anchor, state);
        FieldFx_RotateVector(ANIM_OWNER_AS(RoomM086Channel)->pool->bone, &drift, &state->velocity);
        state->state = 0;
        state->timer = 0;
        state->velocity.y -= 0x18;
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM086Channel)->pool, 0,
                      &state->rotation);
        if (FX_TASK_AS(RoomM086EventState)->active) {
            RoomM086Pool *pool = ANIM_OWNER_AS(RoomM086Channel)->pool;
            if (pool && pool->object) {
                u8 *flag = pool->object->state;
                if (*flag == 1) *flag = 2;
            }
        }
        g_RoomM086SeekerModelA = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5465704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM086SeekerModelA, 0, 0);
        g_RoomM086SeekerModelB = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5865704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)g_RoomM086SeekerModelB, 0, 0);
        FieldFx_PlaySoundAtOwner(0x577, FieldFx_GetOwnerProgress());
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomM086SpawnChannel)->pool, 0x14, 0xE,
                             (FieldFxCallbackListCallback)RoomM086_SeekerCallback);
    case 1:
        switch (state->state) {
        case 0:
            state->timer++;
            state->position.x += state->velocity.x;
            state->position.y += state->velocity.y;
            state->position.z += state->velocity.z;
            state->velocity.x = state->velocity.x * 31 / 32;
            state->velocity.z = state->velocity.z * 31 / 32;
            state->velocity.y += 5;
            if (g_FieldFxCurrentAge == 1) {
                for (i = 0; i < 6; i++) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM086SpawnChannel)->pool);
                    if (child) {
                        child->position.x = state->position.x;
                        child->position.y = state->position.y;
                        child->position.z = state->position.z;
                        child->position.x += (rand() & 0x7F) - 0x40;
                        child->position.y += (rand() & 0x7F) - 0x40;
                        child->position.z += (rand() & 0x7F) - 0x40;
                        child->state = 1;
                        child->timer = 0;
                    }
                }
            }
            if (state->position.y < g_RoomFloorY->y - 0x202) return 0;
            angle = rand();
            for (i = 0; i < 6; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM086SpawnChannel)->pool);
                if (child) {
                    child->position.x = state->position.x;
                    child->position.y = state->position.y;
                    child->position.z = state->position.z;
                    speed = (rand() & 0x1F) + 0x10;
                    child->heading.x = rcos(angle) * speed / 4096;
                    child->heading.z = rsin(angle) * speed / 4096;
                    child->heading.y = -8 - (rand() & 0xF);
                    child->state = 2;
                    child->timer = 0;
                }
                angle += 0x2AA;
            }
            angle = state->rotation.y - (params->spacing * params->count) / 2;
            for (i = 0; i < params->count; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM086SpawnChannel)->pool);
                if (child) {
                    child->position.x = state->position.x;
                    child->position.y = state->position.y;
                    child->position.z = state->position.z;
                    child->heading.x = state->rotation.x;
                    child->heading.y = state->rotation.y;
                    child->heading.z = state->rotation.z;
                    child->heading.x -= 0xC8;
                    child->heading.y = angle;
                    child->state = 0;
                    child->timer = 0;
                }
                angle += params->spacing;
            }
            state->state = 1;
            state->timer = 0;
            break;
        case 1:
            if (++state->timer >= 8) {
                state->state = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->timer++;
            if (state->timer >= 0x40) return 1;
            break;
        }
        break;
    case 2:
        g_FieldFxSpriteParams.depth = 4;
        g_FieldFxSpriteParams.parameter0A = 0;
        size = params->size / 20;
        switch (state->state) {
        case 0:
            rotation.x = state->rotation.x;
            rotation.y = state->rotation.y;
            rotation.z = state->rotation.z;
            rotation.z += state->timer * 128;
            index = &g_FieldFxTpageSlot3;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[*index];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            page = (u16)(g_FieldFxTpageTable[*index] | GetTPage(0, 0xFF, 0, 0));
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_ModelSetTexture(page, GetClut(0x10, palette));
            }
            FieldFx_ModelSetSemiTrans(0);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = state->position.x;
            matrix.t[1] = state->position.y;
            matrix.t[2] = state->position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_RoomM086SeekerModelA);
            FieldFx_ModelScaleColors(g_RoomM086SeekerModelA, 0x80);
            FieldFx_ModelDraw((union FieldTexturedModel *)g_RoomM086SeekerModelA, &matrix);
            FieldFx_ModelRestoreColors(g_RoomM086SeekerModelA);
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            rotation.pad = 1;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            position.y = g_RoomFloorY->y;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&position, (GteRotation *)&rotation, size * 20, size * 20,
                              (s16)g_FieldFxSpriteParams.parameter02 + 0xFD,
                              GetClut(0x80, palette), 2, 0x80, 0);
            }
            break;
        case 1:
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads(&state->position, 0, size * 20, size * 20,
                              (s16)g_FieldFxSpriteParams.parameter02 * state->timer + 0x20,
                              GetClut(0x80, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            break;
        }
        break;
    }
    return 0;
}

/* Bouncing glint particle callback for the room_m086 controller: mode 1
 * moves it (drift, then fall and bounce), mode 2 draws it. Both draws read
 * the palette kind back from the parameter block, which keeps the block
 * address in a saved register for the parameter02 read after the clut call.
 * The switches have no default arms so the last draw falls straight into
 * the shared return label, which lets each phase 0 draw cross-jump only the
 * call. */

int RoomM086_BounceGlintCallback(int mode, RoomBounceGlint *glint, int *size) {
    GteRotation rotation;
    int extent = *size;
    int intensity;

    switch (mode) {
    case 1:
        switch (glint->phase) {
        case 0:
            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vy++;
            if (glint->timer < 24) {
                break;
            }
            return 1;
        case 1: {
            int fall;

            glint->timer++;
            glint->x += glint->vx;
            glint->y += glint->vy;
            glint->z += glint->vz;
            glint->vx = glint->vx * 31 / 32;
            glint->vz = glint->vz * 31 / 32;
            fall = (u16)glint->vy + 4;
            glint->vy = fall;
            if ((s16)glint->y >= g_RoomFloorY->y) {
                int bounce = -(s16)fall;

                glint->vy = bounce;
            }
            if (glint->timer >= 32) {
                return 1;
            }
            break;
        }
        default:
            return 0;
        }
        break;
    case 2:
        switch (glint->phase) {
        case 0:
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            intensity = rcos((glint->timer << 10) / 24) / 32;
            if (glint->flag != 0) {
                int kind;
                int palette;

                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)glint, 0, extent, extent,
                              (s16)g_FieldFxSpriteParams.parameter02 * (s16)(glint->timer / 6),
                              GetClut(0, palette), 2, intensity, 0);
            } else {
                int kind;
                int palette;

                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)glint, 0, extent * 3 / 2,
                              extent * 3 / 2,
                              (s16)g_FieldFxSpriteParams.parameter02 * ((glint->timer / 2) & 3),
                              GetClut(0, palette), 1, intensity, 0);
            }
            break;
        case 1: {
            int kind;
            int palette;

            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            rotation.z = g_FieldFxCurrentAge << 7;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)glint, &rotation, extent * 3 / 2,
                          extent * 3 / 2,
                          (s16)g_FieldFxSpriteParams.parameter02 * glint->flag + 0x1A,
                          GetClut(0x70, palette), 0xFF, 0x80, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

s32 rand(void);

#define COPY_POS(dst, src) \
    (dst)->x = (src)->x;   \
    (dst)->y = (src)->y;   \
    (dst)->z = (src)->z

s32 RoomM086_BounceGlint(s32 mode, RoomBounceGlint *ent) {
    RoomM086Offset blob;
    RoomBounceGlint *actor;
    s32 phase;
    s32 angle;
    s32 mag;
    s32 i;
    s32 value;

    blob = g_RoomM086JointAnchor;

    if (mode != 1) {
        if (mode < 2) {
            if (mode == 0) {
                goto mode0;
            }
            goto ret0;
        }
        if (mode == 2) {
            goto mode2;
        }
        goto ret0;

    mode0:
        FieldFx_TransformMatrixPoint(((void **)ANIM_OWNER_AS(void))[2], 7, &blob, ent);
        ent->y = g_RoomFloorY->y;
        value = FieldFx_GetOwnerProgress();
        FieldFx_PlaySoundAtOwner(0x579, value);
        FieldFx_InitCallbackList(((void **)ANIM_POOL_AS(void))[2], 0x14, 0x18,
                      (FieldFxCallbackListCallback)RoomM086_BounceGlintCallback);
        goto done;

    }
mode1:
    value = g_FieldFxCurrentAge;
    if (value == 1) {
        angle = rand();
        i = 0;
        do {
            actor = FieldFx_AcquireCallbackSlot(((void **)ANIM_POOL_AS(void))[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (rand() & 0xF) + 7;
                value = rcos(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = rsin(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(rand() & 0xF) - 0xE;
                actor->phase = 0;
                actor->timer = 0;
            }
            i++;
            angle += 0x2AA;
        } while (i < 6);
        value = g_FieldFxCurrentAge;
    }

    value = g_FieldFxCurrentAge - 2;
    if (value < 3U) {
        angle = rand();
        i = 0;
        do {
            actor = FieldFx_AcquireCallbackSlot(((void **)ANIM_POOL_AS(void))[2]);
            if (actor != 0) {
                COPY_POS(actor, ent);
                mag = (rand() & 0x1F) + 0x18;
                value = rcos(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vx = value >> 12;
                value = rsin(angle) * mag;
                if (value < 0) {
                    value += 0xFFF;
                }
                actor->vz = value >> 12;
                actor->vy = -(rand() & 0x1F) - 0x18;
                actor->flag = rand() & 1;
                actor->phase = 1;
                actor->timer = 0;
            }
            i++;
            angle += 0xAAA;
        } while (i < 2);
    }

    value = g_FieldFxCurrentAge - 2;
    if (value < 0x15U) {
        angle = rand();
        actor = FieldFx_AcquireCallbackSlot(((void **)ANIM_POOL_AS(void))[2]);
        if (actor != 0) {
            COPY_POS(actor, ent);
            value = rand();
            phase = actor->x;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->x = phase;
            value = rand();
            phase = actor->z;
            phase -= 0x80;
            phase += value & 0xFF;
            actor->z = phase;
            mag = (rand() & 0xF) + 7;
            if (g_FieldFxCurrentAge & 1) {
                actor->vy = -(rand() & 0x1F) - 0x1E;
            } else {
                actor->vy = -(rand() & 0xF) - 8;
                mag = (int)(mag + ((unsigned)mag >> 31)) >> 1;
            }
            value = rcos(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vx = value >> 12;
            value = rsin(angle) * mag;
            if (value < 0) {
                value += 0xFFF;
            }
            actor->vz = value >> 12;
            actor->flag = rand() & 1;
            actor->phase = 0;
            actor->timer = 0;
        }
    }
    if (g_FieldFxCurrentAge >= 0x46) {
        return 1;

    mode2: {
        int index = g_FieldFxTpageSlot3;
        u16 tpage;
        g_FieldFxSpriteParams.depth = 0x10;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        tpage = g_FieldFxTpageTable[index];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.tpage = tpage;
    }

    }
ret0:
    return 0;
done:;
}

#undef COPY_POS

int *RoomM086_SetControllerParams(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        g_RoomM086ControllerParams[0] = arg1;
        g_RoomM086ControllerParams[1] = arg2;
        g_RoomM086ControllerParams[2] = arg3;
    } else if (arg0 == 2) {
        g_RoomM086ControllerParams[3] = arg1;
    }
    return g_RoomM086ControllerParams;
}

int *RoomM086_SetBounceGlintParams(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        int *base = g_RoomM086BounceGlintParams;

        base[0] = arg1;
        base[1] = arg2;
        base[2] = arg3;
        base[0] = 0x1000;
    }
    return g_RoomM086BounceGlintParams;
}
