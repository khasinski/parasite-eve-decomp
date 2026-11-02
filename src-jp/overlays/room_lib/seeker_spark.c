/*
 * The seeker spark of scenes e03, e06, e07 and e27: one function, linked
 * right after the room library. Its rodata is the two colour initialisers.
 */

#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_spark.h"

/* First object of the primary channel's pool: its packet flags. */
typedef struct RoomSeekerObject {
    u32 flags;
} RoomSeekerObject;

typedef struct RoomSeekerPool {
    RoomSeekerObject *object;
} RoomSeekerPool;

/* Seeker spawner parameters: the step length along the heading. */
typedef struct RoomSeekerSparkParams {
    s16 distance;
} RoomSeekerSparkParams;


/* Seeker spark: flies along its heading dropping trail sparks, bursts on
 * the player (flagging the battle actor) and dies off the walkable floor;
 * trail sparks drift, slow and bounce on the floor for eight frames. */
int RoomEffect_SeekerSparkCallback(int mode, RoomDampedSpark *spark,
                                   RoomSeekerSparkParams *params) {
    GteShortVector delta;
    RenderColor color = {0xC8, 0x64, 0x00, 0x00};
    RenderColor glow = {0xC8, 0xC8, 0x96, 0x00};
    RoomDampedSpark *child;
    int intensity;
    int scale;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            FieldFx_AnglesToVector((GteShortVector *)&spark->vx, params->distance, &delta);
            spark->x += delta.x;
            spark->y += delta.y;
            spark->z += delta.z;
            if ((spark->timer & 3) == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child != 0) {
                    child->x = spark->x;
                    child->y = spark->y;
                    child->z = spark->z;
                    child->vx = (rand() & 0x1F) - 0x10;
                    child->vy = 0;
                    child->vz = (rand() & 0x1F) - 0x10;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            FieldFx_AnglesToVector((GteShortVector *)&spark->vx, 0xB4, &delta);
            delta.x += spark->x;
            delta.y += spark->y;
            delta.z += spark->z;
            if (FieldFx_IsPlayerWithinRadius(&delta, 0x32)) {
                if (FX_TASK_AS(RoomSparkEventState)->active) {
                    RoomSparkChannel *channel = ANIM_OWNER_AS(RoomSparkChannel);
                    if ((((RoomSeekerPool *)channel->pool)->object->flags & 0x3F000000) ==
                        0x01000000) {
                        ((RoomSparkBattleEntity *)D_8009CD10)->actor->flags |= 0x4000;
                        ((RoomSeekerPool *)channel->pool)->object->flags =
                            (((RoomSeekerPool *)channel->pool)->object->flags & 0xC0FFFFFF) |
                            0x19000000;
                        ((RoomSeekerPool *)channel->pool)->object->flags |= 0x80000000;
                    }
                }
                spark->x = delta.x;
                spark->y = delta.y;
                spark->z = delta.z;
                spark->state = 2;
                spark->timer = 0;
            }
            if (Geo_PointInPoly(spark->x << 16, spark->z << 16, g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) {
                return 1;
            }
            break;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 2;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer >= 8) return 1;
            break;
        case 2:
            spark->timer++;
            if ((s16)spark->timer >= 8) return 1;
            break;
        }
        break;
    case 2:
        gte_ldrotmatrix(g_ViewMatrixSlot.value);
        gte_ldtransmatrix(g_ViewMatrixSlot.value);
        switch (spark->state) {
        case 0: {
            int clut;
            int kind;
            int palette;

            intensity = ((g_FieldFxCurrentAge & 1) << 5) + 0x60;
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            /* The tpage lookup comes first so its indexed load is
             * scheduled ahead of the parameter stores, as in retail. */
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[4]];
            g_FieldFxSpriteParams.palette = 4;
            g_FieldFxSpriteParams.parameter06 = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            /* The kind is re-read through its own label after the call, so
             * it is not addressed off a saved parameter block base. */
            FieldFx_DrawRotatedQuad((GteShortVector *)spark, (GteRotation *)&spark->vx, 0x140, 0x78, 0,
                          (g_FieldFxSpriteParams.palette == 4 && g_FieldFxAltPaletteFlag != 0) ? 0x60 : 0, 0x40, 0x10,
                          clut, &color, &glow, (s16)intensity, 1);
            if (g_FieldFxCurrentAge & 3) {
                return 0;
            }
            gte_ldrotmatrix(g_ViewMatrixSlot.value);
            gte_ldtransmatrix(g_ViewMatrixSlot.value);
            FieldFx_AnglesToVector((GteShortVector *)&spark->vx, 0xF0, &delta);
            delta.x += spark->x;
            delta.y += spark->y;
            delta.z += spark->z;
            FieldFx_DrawStarFan(&delta, 200, 200, 5, 0, 0x1000, 0x1000, &glow, 0,
                          intensity / 2, 1);
            return 0;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;

            scale = rcos((s16)spark->timer << 7) * 2 / 3;
            intensity = rsin((s16)spark->timer << 7) / 64 + 0x40;
            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter00 = 0x10;
            g_FieldFxSpriteParams.parameter02 = 1;
            g_FieldFxSpriteParams.extent_x = 0x10;
            g_FieldFxSpriteParams.extent_y = 0x10;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0x30, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, scale, scale,
                          (s16)g_FieldFxSpriteParams.parameter02 * (spark->timer & 1) + 0xDC, clut, 1,
                          intensity, &color);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;

            g_FieldFxSpriteParams.tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[2]];
            g_FieldFxSpriteParams.palette = 2;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)spark, 0, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)spark->timer / 2) + 0xC0, clut, 1,
                          0x80, &glow);
            break;
        }
        }
        break;
    }
    return 0;
}
