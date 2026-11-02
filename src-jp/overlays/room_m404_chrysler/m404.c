#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m404.h"
#include "pe1/room_m404_drop.h"
#include "pe1/room_m404_effects.h"

/* Flashes the scene with the given code when the drop touches the player. */
#define ROOM_M404_DROP_FLASH(code)                                            \
    {                                                                         \
        RoomM404DropChannel *channel = ANIM_OWNER_AS(RoomM404DropChannel);                            \
        if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {        \
            ((RoomM404DropEntity *)D_8009CD10)->actor->flags |= 0x4000;                               \
            channel->pool->node->flags =                                      \
                (channel->pool->node->flags & 0xC0FFFFFF) | (code);           \
            channel->pool->node->flags |= 0x80000000;                         \
        }                                                                     \
    }

/* Sinking drop: sinks and jitters until it spreads on the floor, sheds
 * splash children, lingers as a pool that spits droplets and expires once
 * it leaves the walkable area; the splashes slow down and bounce. */
int RoomM404_SinkingDropCallback(int mode, RoomM404Drop *drop) {
    RoomM404DropRotation rotation = g_RoomM404DropFanTilt;
    RoomM404DropColor color = g_RoomM404DropFanColor;
    RoomM404DropColor glow = g_RoomM404DropGlowColor;
    RoomM404DropColor fadeColor;
    RoomM404Drop *child;
    int size;
    int alpha;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (drop->state) {
        case 0:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0xF) - 8;
            drop->z += (rand() & 0xF) - 8;
            drop->y += 0x40;
            if (drop->y >= g_RoomFloorY->y - 0x10) {
                drop->state = 1;
                drop->y = g_RoomFloorY->y - 0x10;
                drop->vy = 0;
                drop->vx *= 2;
                drop->vz *= 2;
            }
            if (drop->timer & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM404DropSpawnChannel)->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->vx = (rand() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (rand() & 0x1F) - 0x10;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (FieldFx_IsPlayerWithinRadius(drop, 0x6E) && drop->y >= g_RoomFloorY->y - 0x202 &&
                FX_TASK_AS(RoomM404DropEvent)->active) {
                ROOM_M404_DROP_FLASH(0x3D000000);
            }
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) return 1;
            break;
        case 1:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0x1F) - 0x10;
            drop->z += (rand() & 0x1F) - 0x10;
            if (drop->timer & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM404DropSpawnChannel)->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->vx = (rand() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (rand() & 0x1F) - 0x10;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (FieldFx_IsPlayerWithinRadius(drop, 0x6E) && FX_TASK_AS(RoomM404DropEvent)->active) {
                ROOM_M404_DROP_FLASH(0x19000000);
            }
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 2:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 127 / 128;
            drop->vz = drop->vz * 127 / 128;
            fall = (u16)drop->vy - 1;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer < 0x10) return 0;
            drop->state = 4;
            drop->timer = 0;
            drop->y = g_RoomFloorY->y;
            break;
        case 3:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 511 / 512;
            drop->vz = drop->vz * 511 / 512;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0xC) return 1;
            break;
        case 4:
            drop->timer++;
            if (FieldFx_IsPlayerWithinRadius(drop, 0x6E) && FX_TASK_AS(RoomM404DropEvent)->active) {
                ROOM_M404_DROP_FLASH(0x19000000);
            }
            if ((drop->timer & 1) && (rand() & 0xF) == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomM404DropSpawnChannel)->pool);
                if (child) {
                    child->x = drop->x;
                    child->y = drop->y;
                    child->z = drop->z;
                    child->x += (rand() & 0x7F) - 0x40;
                    child->y -= rand() & 0x3F;
                    child->z += (rand() & 0x7F) - 0x40;
                    child->vx = (rand() & 0x1F) - 0x10;
                    child->vy = -(rand() & 7);
                    child->vz = (rand() & 0x1F) - 0x10;
                    child->state = 3;
                    child->timer = 0;
                }
            }
            if (drop->timer >= 0x2E) return 1;
            break;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *slot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
        }
        switch (drop->state) {
        case 0:
        case 1:
            size = rsin(g_FieldFxCurrentAge << 9) / 4 + 0x1000;
            alpha = (g_FieldFxCurrentAge & 1) * 48 + 0x80;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, size, size, 0x20, GetClut(0x10, palette), 1, alpha,
                              (struct RenderColor *)&glow);
            }
            if (rand() & 3) return 0;
            {
                RenderMatrixSlot *slot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            FieldFx_DrawStarFan((GteShortVector *)drop, 0xC8, 0xC8, 5, 0, size, size, (struct RenderColor *)&glow, 0, alpha, 1);
            break;
        case 2:
            FieldFx_SampleColorTrack(g_RoomM404SinkingDropColorTrack, &fadeColor, (drop->timer << 4) / 12);
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, 0x1000, 0x1000,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 2) + 0x20,
                              GetClut(0x10, palette), 1, 0x80, (struct RenderColor *)&fadeColor);
            }
            return 0;
        case 3:
            size = rsin(drop->timer << 6) * 4 / 9 + 0x1000;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, size, size,
                              (s16)g_FieldFxSpriteParams.parameter02 * (drop->timer / 2),
                              GetClut(0, palette), 2, 0x20, 0);
            }
            return 0;
        case 4:
            alpha = rsin((drop->timer << 11) / 46) / 32;
            size = drop->timer & 0xF;
            if (size >= 8) size += 0x18;
            g_FieldFxSpriteParams.extent_x = g_FieldFxSpriteParams.parameter00;
            g_FieldFxSpriteParams.extent_y = g_FieldFxSpriteParams.parameter00 * 2;
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
                FieldFx_DrawShapeQuads((GteShortVector *)drop, 0, 0x1800, 0x1000,
                              (s16)g_FieldFxSpriteParams.parameter02 * size + 0x40,
                              GetClut(0x20, palette), 1, alpha, 0);
            }
            g_FieldFxSpriteParams.extent_x = g_FieldFxSpriteParams.parameter00;
            g_FieldFxSpriteParams.extent_y = g_FieldFxSpriteParams.parameter00;
            {
                RenderMatrixSlot *slot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            if (rand() & 3) return 0;
            FieldFx_DrawStarFan((GteShortVector *)drop, 0x140, 0x140, 6, (GteRotation *)&rotation, 0x1000, 0x1000, (struct RenderColor *)&color, 0,
                          alpha / 2, 1);
            break;
        }
        break;
    }
    return 0;
}

#undef ROOM_M404_DROP_FLASH

int RoomM404_SinkingDrop(int mode, void *unused, char *state) {
    RoomTemplate8 template = g_RoomM404SinkingDropAnchor;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomM404Particle *child;
    switch (mode) {
    case 0:
        {
            int handle = FieldFx_GetOwnerProgress();
            FieldFx_PlaySoundAtOwner(0x5b9, handle);
            if (FX_TASK_AS(RoomM404EventState)->active) {
                RoomM404Node **slot = (RoomM404Node **)ANIM_OWNER_AS(RoomM404Channel)->pool;
                if (slot && *slot) {
                    u8 *flag = (*slot)->state;
                    if (*flag == 1) *flag = 2;
                }
            }
            pool = ANIM_POOL_AS(RoomM404Channel)->pool;
            return FieldFx_InitCallbackList(pool, 20, 55,
                                 (FieldFxCallbackListCallback)RoomM404_SinkingDropCallback);
        }
    case 1:
    {
        pool = ANIM_OWNER_AS(RoomM404Channel)->pool;
        FieldFx_TransformMatrixPoint(pool, 3, &template, position);
        pool = ANIM_OWNER_AS(RoomM404Channel)->pool;
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)pool, 0,
                      (GteShortVector *)target);
        if (g_FieldFxCurrentAge == 1) {
            pool = ANIM_POOL_AS(RoomM404Channel)->pool;
            child = FieldFx_AcquireCallbackSlot(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                FieldFx_AnglesToVector((GteShortVector *)target, *(s16 *)state, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
        }
        if (g_FieldFxCurrentAge < 2) break;
        return 2;
    }
    case 2:
    {
        int idx = g_FieldFxTpageSlot3;
        int palette;
        g_FieldFxSpriteParams.parameter00 = 32;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 32;
        g_FieldFxSpriteParams.extent_y = 32;
        palette = g_FieldFxTpageTable[idx];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        g_FieldFxSpriteParams.tpage = palette;
    }
    }
    return 0;
}

/* Spark of the m404 joint debris burst: state 0 bounces twice off the
 * floor while circling and draws as two glow points, state 1 falls
 * and draws as a spinning sprite tinted by a color track. */
int RoomM404_DebrisSparkCallback(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = g_RoomM404DebrisSparkRotation;
    GteShortVector position;
    RenderColor trackColor;
    RenderColor color = g_RoomM404DebrisSparkColor;
    RenderMatrixSlot *matrixSlot;
    int fall;
    int bounce;
    int scale;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 255 / 256;
            spark->vz = spark->vz * 255 / 256;
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
                if (spark->y >= g_RoomFloorY->y) {
                    spark->vy = (s16)bounce / 4;
                }
            }
            spark->z += rsin((s16)spark->timer << 9) / 512;
            spark->x += rcos((s16)spark->timer << 9) / 512;
            if ((s16)spark->timer < 64) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy + 2;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                int rebound = -(s16)fall;
                spark->vy = rebound;
            }
            if ((s16)spark->timer < 32) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0:
            matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            FieldFx_DrawPointTile((GteShortVector *)spark, (unsigned char *)&color, 40, 0xFF);
            position.x = spark->x;
            position.y = spark->y;
            position.z = spark->z;
            position.y += 8;
            FieldFx_DrawPointTile(&position, (unsigned char *)&color, 40, 0xFF);
            break;
        case 1: {
            int clut;
            int kind;
            int palette;
            scale = rcos((s16)spark->timer << 5) + 0x1000;
            g_FieldFxSpriteParams.parameter00 = 32;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 32;
            g_FieldFxSpriteParams.extent_y = 32;
            rotation.z = g_FieldFxCurrentAge << 5;
            FieldFx_SampleColorTrack(g_RoomM404DebrisSparkColorTrack, &trackColor, (s16)spark->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, &rotation, scale / 2, scale / 2,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)spark->timer / 4),
                          clut, 1, 0x60, &trackColor);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Bursts six bouncing sparks and two falling ones from a model joint
 * during the first frames of the effect. */
int RoomM404_DebrisBurst(int mode, RoomM404BurstOrigin *origin) {
    RoomM404JointTemplate template = g_RoomM404DebrisBurstAnchor;
    RoomDampedSpark *spark;
    int i;

    switch (mode) {
    case 0:
        FieldFx_GetActorPosition(ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, (s16 *)origin);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, &template, origin);
        FieldFx_PlaySoundAtOwner(0x5C0, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5C1, 0x80);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 32,
                             (FieldFxCallbackListCallback)RoomM404_DebrisSparkCallback);
    case 1:
        if (g_FieldFxCurrentAge < 5) {
            for (i = 0; i < 6; i++) {
                spark = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (spark != 0) {
                    spark->x = origin->x;
                    spark->y = origin->y;
                    spark->z = origin->z;
                    spark->vx = rand() % 50 - 25;
                    spark->vy = -(rand() % 39);
                    spark->vz = rand() % 50 - 25;
                    spark->state = 1;
                    spark->timer = 0;
                }
            }
            for (i = 0; i < 2; i++) {
                spark = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (spark != 0) {
                    spark->x = origin->x;
                    spark->y = origin->y;
                    spark->z = origin->z;
                    spark->vx = rand() % 80 - 40;
                    spark->vy = -(rand() % 80);
                    spark->vz = rand() % 80 - 40;
                    spark->state = 0;
                    spark->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 2) break;
        return 2;
    case 2:
        g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
        g_FieldFxSpriteParams.palette = 3;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

/* The stack templates the effects above copy: read-only data of this unit,
 * which GCC emits after the unit's jump tables, as retail has them. */
const RoomTemplate8 g_RoomM404SinkingDropAnchor = {{0, 0xFFE2}};
const GteRotation g_RoomM404DebrisSparkRotation = {0, 0, 0, 0};
const RenderColor g_RoomM404DebrisSparkColor = {0x6E, 0x3C, 0x3C, 0};
const RoomM404JointTemplate g_RoomM404DebrisBurstAnchor = {{0, 0x2BC}};
