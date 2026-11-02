#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/room_floor.h"
#include "pe1/scene_e22_quake_blast.h"
#include "pe1/scene_e22_ring_burst.h"

#include "scene_e22_ember_data.h"
#include "scene_e22_shared.h"

typedef struct SceneE22Effect {
    u16 x, y, z;
    s16 speed;
    s16 state;
    s16 frame;
} SceneE22Effect;

typedef struct { char reserved[0x14]; short complete, signaled; } WaitState;

/* Ring burst particle: sweeps out from the orbit centre drawing a coloured
 * trail, flashes as a spinning sprite, or drifts as a widening ring; the
 * update side drifts and bounces the flashing ones on the floor. */
int SceneE22_RingBurstParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteShortVector offset;
    GteRotation spin;
    RenderColor color;
    RenderColor seed;
    int fall;
    int bounce;
    int scale;
    int size;
    int glow;
    int radius;

    seed = g_SceneE22RingBurstTriangleColor;
    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 4;
            p->heading.y += 0x18;
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y + 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.x += 2;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            scale = (rsin(((s16)p->timer << 10) / 24) + 0x1000) / 2;
            FieldFx_AnglesToVector(&p->heading, (s16)(p->radius * scale / 4096), &offset);
            offset.x += g_SceneE22BurstAnchor.x;
            offset.y += g_SceneE22BurstAnchor.y;
            offset.z += g_SceneE22BurstAnchor.z;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            FieldFx_SampleColorTrack(g_SceneE22RingBurstParticleColorTrack, &color, (s16)p->timer);
            FieldFx_DrawGlowLine(&g_SceneE22BurstAnchor, &offset, &color, 0, 0x80, 0, 1);
            break;
        case 1: {
            u16 clut;
            int kind;
            int palette;
            size = rsin((s16)p->timer << 6) + 0x800;
            glow = rcos((s16)p->timer * 1204 / 16) / 128 + 0x28;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            spin.x = 0;
            spin.y = 0;
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            clut = GetClut(0, palette);
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)p->timer / 2) + 0x80,
                          clut, 1, glow, 0);
            break;
        }
        case 2:
            glow = rcos((s16)p->timer << 7) / 32;
            scale = rsin((s16)p->timer << 7) / 4 + 0xC00;
            radius = p->radius * scale / 4096;
            size = rcos((s16)p->timer << 7) * 240 / 4096;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            FieldFx_DrawRotatedTriangle(&g_SceneE22BurstAnchor, (GteRotation *)&p->heading, radius, size, &seed, 0, 0,
                          (s16)glow, 1);
            break;
        }
        break;
    }
    return 0;
}

/* Ring burst: while the scene event runs it bursts falling sparks, an
 * optional volley of sweeping trails and drifting rings from the anchor
 * during the first three frames, and flashes a glow, a halo and a tilted
 * ring while it fades; the anchor is published as the orbit centre. */
int SceneE22_RingBurst(int mode, RoomOrbitRingBurst *burst) {
    GteRotation tilt;
    RenderColor ringColor = g_SceneE22RingBurstRingColor;
    RenderColor haloColor = g_SceneE22RingBurstHaloColor;
    RoomOrbitTrailParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->counterB = 0;
        burst->counterA = 0;
        burst->sweep = 1;
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 0, (s16 *)burst);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22BurstChannel)->pool, 0x14, 0x18,
                             (FieldFxCallbackListCallback)SceneE22_RingBurstParticleCallback);
    case 1:
        if (FX_TASK_AS(SceneE22BurstEvent)->running == 0)
            return 1;
        if (g_FieldFxCurrentAge == 2) {
            FieldFx_PlaySoundAtOwner(0x5D4, FieldFx_GetOwnerProgress());
            FieldFx_PlaySoundAtOwner(0x5D5, 0x80);
        }
        if (g_FieldFxCurrentAge < 3) {
            for (i = 0; i < 2; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22BurstChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand() % 60 - 0x1E;
                    child->heading.y = rand() % 60 - 0x1E;
                    child->heading.z = rand() % 60 - 0x1E;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (burst->sweep) {
                for (i = 0; i < 5; i++) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22BurstChannel)->pool);
                    if (child) {
                        child->x = burst->x;
                        child->y = burst->y;
                        child->z = burst->z;
                        child->heading.x = rand();
                        child->heading.y = rand();
                        child->heading.z = rand();
                        child->radius = (rand() & 0x3FF) + 0x200;
                        child->state = 0;
                        child->timer = 0;
                    }
                }
            }
            for (i = 0; i < 5; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22BurstChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->heading.x = rand();
                    child->heading.y = rand();
                    child->heading.z = rand();
                    child->radius = (rand() & 0x7FF) + 0x400;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x1C) break;
        return 1;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x40;
        g_FieldFxSpriteParams.parameter02 = 4;
        g_FieldFxSpriteParams.extent_x = 0x40;
        g_FieldFxSpriteParams.extent_y = 0x40;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x20;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (g_FieldFxCurrentAge < 0x15) {
            int kind;
            int palette;
            int glow = rcos((g_FieldFxCurrentAge << 10) / 20) / 32;
            int size = rsin((g_FieldFxCurrentAge << 10) / 20) + 0x1000;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            palette = GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7 : palette + 3);
            FieldFx_DrawShapeQuads((GteShortVector *)burst, 0, size, size, 4, (u16)palette, 1,
                          glow / 2, 0);
            FieldFx_DrawStarFan((GteShortVector *)burst, 800, 800, 12, 0, 0x1000, 0x1000,
                          &haloColor, 0, glow, 1);
            tilt.x = 0x400;
            tilt.y = 0;
            tilt.z = 0;
            tilt.flags = 1;
            /* The ring scale reuses the glow sprite's size temporary. */
            size = rsin((g_FieldFxCurrentAge << 10) / 20);
            FieldFx_DrawRingBand((GteShortVector *)burst, 0x44C, 0x578, 0x14, &tilt, size,
                          size, 0, &ringColor, glow / 2, 1);
        }
        g_SceneE22BurstAnchor.x = burst->x;
        g_SceneE22BurstAnchor.y = burst->y;
        g_SceneE22BurstAnchor.z = burst->z;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 8;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Damped glow particle: drifts with a slowing velocity and bounces on the
 * floor for 8 frames, drawing a pulsing glow sprite tinted by its colour
 * track. */
int SceneE22_LinkBeamSparkCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    RenderColor color;
    int fall;
    int bounce;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        if (p->state != 0) return 0;
        p->timer++;
        p->x += p->heading.x;
        p->y += p->heading.y;
        p->z += p->heading.z;
        p->heading.x = p->heading.x * 63 / 64;
        p->heading.z = p->heading.z * 63 / 64;
        fall = (u16)p->heading.y;
        p->heading.y = fall;
        if (p->y >= g_RoomFloorY->y) {
            bounce = -(s16)fall;
            p->heading.y = bounce;
        }
        if ((s16)p->timer < 8) break;
        return 1;
    case 2:
        if (p->state != 0) return 0;
        angle = (s16)p->timer << 7;
        size = rcos(angle) / 2 + 0x800;
        glow = rcos(angle) / 32;
        spin.x = 0;
        spin.y = 0;
        spin.z = (s16)p->timer * 32;
        spin.flags = 0;
        FieldFx_SampleColorTrack(g_SceneE22LinkBeamSparkColorTrack, &color, (s16)p->timer * 2);
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int kind = g_FieldFxSpriteParams.palette;
            int palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 + 100,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8 : palette + 4),
                          1, glow, &color);
        }
        break;
    }
    return 0;
}

/* Link beam controller: strings two light ribbons across three actor
 * joints; for 33 frames the last joint sheds a glow spark along its
 * heading, after which the ribbons fade out. */
int SceneE22_LinkBeam(int mode, SceneE22LinkBeam *beam) {
    GteShortVector first;
    GteShortVector middle;
    GteShortVector last;
    GteShortVector heading;
    GteShortVector offset0 = g_SceneE22JointOrigin;
    GteShortVector offset1 = g_SceneE22JointAnchor;
    RenderColor color0 = g_SceneE22LinkBeamTailColor;
    RenderColor color1 = g_SceneE22LinkBeamMidColor;
    RenderColor color2 = g_SceneE22LinkBeamHeadColor;
    RoomOrbitTrailParticle *spark;

    switch (mode) {
    case 0:
        beam->alpha = 0x80;
        beam->reserved00 = 0;
        beam->timer = 0;
        beam->jointSet = FX_TASK_AS(SceneE22EmberEvent)->jointSet;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].first,
                      &offset0, &first);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].middle,
                      &offset0, &middle);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].last,
                      &offset1, &last);
        FieldFx_DrawHistoryTrail(&first, &middle, 0x3F2, (unsigned char *)&color2, (unsigned char *)&color1, 0x80,
                      (struct FieldTrailPair *)beam->trailA, 1);
        FieldFx_DrawHistoryTrail(&middle, &last, 0x3F2, (unsigned char *)&color1, (unsigned char *)&color0, 0x80,
                      (struct FieldTrailPair *)beam->trailB, 1);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 8,
                             (FieldFxCallbackListCallback)SceneE22_LinkBeamSparkCallback);
    case 1:
        beam->timer++;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].last,
                      &offset1, &first);
        FieldFx_GetTransformLookAngles(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].last,
                      &heading);
        if (beam->timer < 0x21) {
            spark = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (spark != 0) {
                spark->x = first.x;
                spark->y = first.y;
                spark->z = first.z;
                FieldFx_AnglesToVector(&heading, 0x32, &spark->heading);
                spark->state = 0;
                spark->timer = 0;
            }
        } else if (beam->alpha > 0) {
            beam->alpha -= 0x10;
        }
        if (g_FieldFxCurrentAge == 4)
            FieldFx_PlaySoundAtOwner(0x5D3, FieldFx_GetOwnerProgress());
        if (beam->timer < 0x20) break;
        return 2;
    case 2:
        if (beam->alpha >= 0) {
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].first,
                          &offset0, &first);
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].middle,
                          &offset0, &middle);
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, g_SceneE22LinkBeamJoints[beam->jointSet].last,
                          &offset1, &last);
            FieldFx_DrawHistoryTrail(&first, &middle, 10, (unsigned char *)&color2, (unsigned char *)&color1, beam->alpha,
                          (struct FieldTrailPair *)beam->trailA, 1);
            FieldFx_DrawHistoryTrail(&middle, &last, 10, (unsigned char *)&color1, (unsigned char *)&color0, beam->alpha,
                          (struct FieldTrailPair *)beam->trailB, 1);
        }
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.depth = 0x10;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Ember drift particle: embers drift and bounce off the floor while their
 * sideways speed decays (state 0 slowly, state 1 quickly while falling),
 * and a wide glow pulses in place (state 2). */
int SceneE22_EmberDriftParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_SceneE22ZeroSpin;
    GteRotation turn = g_SceneE22FlatTilt;
    RenderColor color = g_SceneE22EmberParticleColor;
    int fall;
    int bounce;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = rsin((s16)p->timer << 7) * 3 / 2;
            glow = rsin((s16)p->timer << 7) / 160;
            spin.z = -((s16)p->timer * 32);
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3)];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_SceneE22EmberDriftColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 * ((s16)p->timer / 2) + 0x80,
                          GetClut(0, palette), 2, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = rcos(((s16)p->timer << 10) / 24);
            spin.z = (s16)p->timer * 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_SceneE22EmberDriftColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size / 2, size / 2, 0x44,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 10
                                                                    : palette + 6),
                          1, 0x80, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            glow = rsin(((s16)p->timer << 11) / 24) / 32;
            size = rcos(((s16)p->timer << 10) / 24) / 2 + 0x800;
            turn.z = g_FieldFxCurrentAge << 6;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_SceneE22EmberDriftColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &turn, size * 2, size * 2, 0x40,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6
                                                                    : palette + 2),
                          1, glow / 2, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember drift controller: rides the room actor and glows up for 16
 * frames, then for 32 frames shakes the camera while it sprays orbiting
 * sparks, embers on odd frames and smoke every sixth; at frame 25 it hands
 * the actor to the scene script; then it fades. While it glows it draws a
 * flickering halo and ring. */
int SceneE22_EmberDrift(int mode, SceneE22EmberBurst *burst) {
    RenderColor ringColor = g_SceneE22EmberDriftRingColor;
    RenderColor haloColor = g_SceneE22EmberDriftHaloColor;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    int glow;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        FieldFx_PlaySoundAtOwner(0x5DB, FieldFx_GetOwnerProgress());
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 1, (s16 *)burst);
        burst->y = g_RoomFloorY->y;
        if (FX_TASK_AS(SceneE22EmberEvent)->active) {
            pool = ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)SceneE22_EmberDriftParticleCallback);
    case 1:
        switch (burst->state) {
        case 0:
            burst->timer++;
            Battle_HaltOnPositiveX();
            burst->glow = rsin(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            if (burst->timer < 0x18)
                Battle_HaltOnPositiveX();
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->heading.x = rand() % 50 - 25;
                child->heading.y = -(rand() % 32);
                child->heading.z = rand() % 50 - 25;
                child->state = 1;
                child->timer = 0;
            }
            if (burst->timer & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->y -= rand() & 0x1FF;
                    child->x += (rand() & 0x7F) - 0x40;
                    child->z += (rand() & 0x7F) - 0x40;
                    child->heading.x = rand() % 24 - 12;
                    child->heading.y = rand() % 24 - 12;
                    child->heading.z = rand() % 24 - 12;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (burst->timer % 6 == 0) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (burst->timer == 0x18)
                Battle_InitEntityColors();
            if (burst->timer == 0x19 && FX_TASK_AS(SceneE22EmberEvent)->active) {
                channel = ANIM_OWNER_AS(SceneE22EmberObjectChannel);
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    g_PlayerEntity->state->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            if (burst->timer < 0x20) break;
            burst->state = 2;
            burst->timer = 0;
            break;
        case 2:
            burst->timer++;
            burst->glow = rcos((burst->timer << 10) / 24) / 32;
            if (burst->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3)];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (burst->glow != 0) {
            RenderColor *color;
            int scale;
            glow = burst->glow;
            color = (g_FieldFxCurrentAge & 1) ? &ringColor : &haloColor;
            scale = 0x1000;
            g_FieldFxSpriteParams.depth = 0x28;
            FieldFx_DrawStarFan((GteShortVector *)burst, 400, 400, 0x10, 0, scale, scale,
                          color, 0, glow, 1);
            FieldFx_DrawRingBand((GteShortVector *)burst, 500, 600, 0x18, 0, scale, scale, 0,
                          color, glow / 2, 3);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Rising ember particle: an ember drifts on a slowly turning circle while
 * it bounces off the floor (state 0), sparks coast to rest (states 1 and
 * 2); each draws as a turning glow. */
int SceneE22_EmberColumnParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin;
    RenderColor color = g_SceneE22EmberParticleColor;
    int fall;
    int bounce;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            p->x += rsin((s16)p->timer << 8) * 40 / 4096;
            p->y += rcos((s16)p->timer << 8) * 40 / 4096;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.y = p->heading.y * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            if ((s16)p->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = rcos((s16)p->timer << 5) / 2 + 0x800;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11)];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            spin.z = (s16)p->timer * 32;
            spin.flags = 0;
            FieldFx_SampleColorTrack(g_SceneE22EmberColumnColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size, size,
                          (s16)g_FieldFxSpriteParams.parameter02 + 0x64,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                         : palette + 4),
                          1, 0x80, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = rsin((s16)p->timer << 6) + 0x800;
            glow = rcos((s16)p->timer * 1204 / 16) / 128;
            g_FieldFxSpriteParams.parameter00 = 0x40;
            g_FieldFxSpriteParams.parameter02 = 4;
            g_FieldFxSpriteParams.extent_x = 0x40;
            g_FieldFxSpriteParams.extent_y = 0x40;
            {
                int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11)];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                spin.x = 0;
                spin.y = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            FieldFx_SampleColorTrack(g_SceneE22EmberColumnColorTrack, &color, ((s16)p->timer << 5) / 12);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, size * 2, size * 2, 0x40,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 6
                                                                         : palette + 2),
                          1, glow, &color);
            break;
        }
        case 2: {
            int kind;
            int palette;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3)];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                spin.x = 0;
                spin.y = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            spin.z = (s16)p->timer * 60;
            spin.flags = 0;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads((GteShortVector *)p, &spin, 0x1800, 0x1800,
                          (s16)g_FieldFxSpriteParams.parameter02 * (s16)p->timer + 0x80,
                          GetClut(0, palette), 1, 0x40, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ember column controller: rides the room actor and for 32 frames shakes
 * the camera while it sprays rising embers, coasting sparks and (on odd
 * frames) slow sparks; at frame 29 it hands the actor to the scene
 * script. It draws a flare, a halo and a floor ring while it burns. */
int SceneE22_EmberColumn(int mode, GteShortVector *anchor) {
    GteRotation spin = g_SceneE22FlatTilt;
    GteShortVector position;
    RenderColor color = g_SceneE22EmberColumnColor;
    RenderColor shade;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    int glow;
    int size;
    int i;

    switch (mode) {
    case 0:
        FieldFx_PlaySoundAtOwner(0x5DC, FieldFx_GetOwnerProgress());
        FieldFx_PlaySoundAtOwner(0x5DD, 0x80);
        FieldFx_GetActorPosition((char *)g_PlayerEntity, 0, (s16 *)anchor);
        if (FX_TASK_AS(SceneE22EmberEvent)->active) {
            pool = ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x3C,
                             (FieldFxCallbackListCallback)SceneE22_EmberColumnParticleCallback);
    case 1:
        if (g_FieldFxCurrentAge < 0x1C)
            Battle_HaltOnPositiveX();
        if (g_FieldFxCurrentAge < 0x11) {
            for (i = 0; i < 2; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = anchor->x;
                    child->y = anchor->y;
                    child->z = anchor->z;
                    child->heading.x = rand() % 64 - 0x20;
                    child->heading.y = rand() % 64 - 0x20;
                    child->heading.z = rand() % 64 - 0x20;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge < 0x21) {
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (child) {
                child->x = anchor->x;
                child->y = anchor->y;
                child->z = anchor->z;
                child->heading.x = rand() % 40 - 20;
                child->heading.y = rand() % 32 - 16;
                child->heading.z = rand() % 40 - 20;
                child->state = 1;
                child->timer = 0;
            }
            if (g_FieldFxCurrentAge & 1) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = anchor->x;
                    child->y = anchor->y;
                    child->z = anchor->z;
                    child->heading.x = rand() % 48 - 24;
                    child->heading.y = rand() % 48 - 24;
                    child->heading.z = rand() % 48 - 24;
                    child->state = 2;
                    child->timer = 0;
                }
            }
        }
        if (g_FieldFxCurrentAge == 0x1C)
            Battle_InitEntityColors();
        if (g_FieldFxCurrentAge == 0x1D && FX_TASK_AS(SceneE22EmberEvent)->active) {
            channel = ANIM_OWNER_AS(SceneE22EmberObjectChannel);
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                g_PlayerEntity->state->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (g_FieldFxCurrentAge < 0x20) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11)];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 5;
            g_FieldFxSpriteParams.depth = 0x20;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        if (g_FieldFxCurrentAge < 0x21) {
            int kind;
            int palette;
            glow = rsin(g_FieldFxCurrentAge << 6) / 32;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 15 / 16;
            size = rsin(g_FieldFxCurrentAge << 5) / 2 + 0x800;
            FieldFx_GetActorPosition((char *)ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, 0, (s16 *)&position);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&position, 0, size * 3 / 2, size * 3 / 2, 0x46,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 11
                                                                         : palette + 7),
                          1, glow, 0);
            FieldFx_DrawStarFan(&position, 800, 600, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            size = rsin(g_FieldFxCurrentAge << 5);
            position.x = anchor->x;
            position.y = anchor->y;
            position.z = anchor->z;
            position.y = g_RoomFloorY->y;
            FieldFx_SampleColorTrack(g_SceneE22EmberColumnColorTrack, &shade, g_FieldFxCurrentAge);
            FieldFx_DrawRingBand(&position, 600, 900, 0x14, &spin, size, size, 0, &shade,
                          glow / 2, 1);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 8;
        break;
    }
    return 0;
}

/* Swirl ring particle: a ring that grows and fades for 12 frames while its
 * heading turns (state 0), or a glow that swings out along its turning
 * heading for 24 frames (state 1). */
int SceneE22_SwirlRingParticleCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_SceneE22ZeroSpin;
    GteShortVector offset;
    RenderColor color;
    int angle;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.x += 8;
            p->heading.y += 0xC;
            if ((s16)p->timer < 0xC) break;
            return 1;
        case 1:
            p->timer++;
            p->heading.x += 0x1D;
            p->heading.y += 0xB;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = rcos(((s16)p->timer << 10) / 12) / 8;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            FieldFx_SampleColorTrack(g_SceneE22SwirlRingParticleColorTrack, &color, (s16)p->timer);
            FieldFx_DrawRotatedTriangle((GteShortVector *)p, (GteRotation *)&p->heading, 0x7D0, size, &color, 0, 0, 0x80, 1);
            return 0;
        case 1:
            angle = ((s16)p->timer << 10) / 24;
            FieldFx_AnglesToVector(&p->heading, (s16)(rcos(angle) * p->radius / 4096),
                          &offset);
            offset.x += p->x;
            offset.y += p->y;
            offset.z += p->z;
            size = rcos(angle) * 2;
            spin.z = (s16)p->timer * 32;
            glow = rsin(angle) / 32;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            {
                int kind = g_FieldFxSpriteParams.palette;
                int palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&offset, &spin, size, size, (s16)g_FieldFxSpriteParams.parameter02 + 100,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                              : palette + 4),
                              1, glow, 0);
            }
            break;
        }
        break;
    }
    return 0;
}

/* Swirl ring controller: rides an actor joint and swells a glow for 30
 * frames, spraying four swirl rings per frame for the first eight, then
 * bursts eight swirling glows and fades for 20 frames. It draws a halo
 * and a spinning flare while it swells, a flashing ring while it fades. */
int SceneE22_SwirlRing(int mode, SceneE22SwirlRing *swirl) {
    GteShortVector offset = g_SceneE22JointAnchor;
    GteShortVector position;
    GteRotation spin = g_SceneE22ZeroSpin;
    RenderColor color = g_SceneE22SwirlRingColor;
    RoomOrbitTrailParticle *child;
    void **soundSlot;
    int volume;
    int angle;
    int glow;
    int i;

    switch (mode) {
    case 0:
        swirl->state = 0;
        swirl->timer = 0;
        swirl->glow = 0;
        swirl->ring = 0;
        soundSlot = g_LoadedSceneAssetBlock;
        if (*soundSlot != 0) {
            volume = 0x7F;
            Akao_SendTableCommand(*soundSlot, 0x5D6, FieldFx_GetOwnerProgress(), 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5D7, 0x80, 0x80, volume);
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, 0, &offset, swirl);
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)SceneE22_SwirlRingParticleCallback);
    case 1:
        position.x = swirl->x;
        position.y = swirl->y;
        position.z = swirl->z;
        switch (swirl->state) {
        case 0:
            swirl->timer++;
            angle = (swirl->timer << 10) / 30;
            swirl->glow = rsin(angle) / 32;
            swirl->ring = rcos(angle) + 0x400;
            if (swirl->timer < 9) {
                for (i = 0; i < 4; i++) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                    if (child) {
                        child->x = position.x;
                        child->y = position.y;
                        child->z = position.z;
                        child->x += (rand() & 0xFF) - 0x80;
                        child->y += (rand() & 0xFF) - 0x80;
                        child->z += (rand() & 0xFF) - 0x80;
                        child->radius = (rand() & 0x3FF) + 0x578;
                        child->heading.x = rand();
                        child->heading.y = rand();
                        child->heading.z = rand();
                        child->state = 1;
                        child->timer = 0;
                    }
                }
            }
            if (swirl->timer < 0x1E) break;
            swirl->timer = 0;
            swirl->state = 1;
            for (i = 0; i < 8; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = position.x;
                    child->y = position.y;
                    child->z = position.z;
                    child->heading.x = rand();
                    child->heading.y = rand();
                    child->heading.z = rand();
                    child->state = 0;
                    child->timer = 0;
                }
            }
            break;
        case 1:
            swirl->timer++;
            angle = (swirl->timer << 10) / 20;
            swirl->glow = rcos(angle) / 32;
            swirl->ring = rsin(angle);
            if (swirl->timer < 0x14) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        position.x = swirl->x;
        position.y = swirl->y;
        position.z = swirl->z;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        switch (swirl->state) {
        case 0: {
            int kind;
            int palette;
            glow = swirl->glow;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            g_FieldFxSpriteParams.depth = 0x20;
            FieldFx_DrawStarFan(&position, 800, 700, 0x10, 0, 0x1000, 0x1000, &color, 0,
                          glow, 1);
            spin.z = swirl->timer * 16;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads((GteShortVector *)swirl, &spin, 0x1000, 0x1000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (swirl->timer & 1) + 0x64,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                         : palette + 4),
                          1, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            int size;
            int timer = swirl->timer;
            if (timer == 1)
                FieldFx_DrawScreenTint((unsigned char *)&color, 0x80, 2, 8);
            else if (timer < 6)
                FieldFx_DrawScreenTint((unsigned char *)&color, rsin(((timer - 2) << 10) / 3) / 32, 1, 8);
            glow = swirl->glow;
            g_FieldFxSpriteParams.depth = 0x20;
            FieldFx_DrawRingBand(&position, 0x44C, 0x5DC, 0x18, 0, swirl->ring, swirl->ring, 0,
                          &color, glow / 2, 1);
            size = swirl->ring + 0x1000;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&position, 0, size, size, 0x44,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 10
                                                                         : palette + 6),
                          1, glow, 0);
            break;
        }
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Twisting trail particle: for 32 frames it turns its twist angle and
 * draws a glow at the end of a bent trail that swings out and back. */
int SceneE22_TwistTrailCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_SceneE22ZeroSpin;
    GteShortVector offset;
    RenderColor color;
    int angle;
    int size;

    switch (mode) {
    case 1:
        if (p->state != 0) return 0;
        p->timer++;
        p->heading.pad += 0x20;
        if ((s16)p->timer < 0x20) break;
        return 1;
    case 2: {
        int kind;
        int palette;
        if (p->state != 0) return 0;
        angle = (s16)p->timer << 5;
        FieldFx_OffsetPointByAngles(p, &offset, 200, &p->heading,
                      rsin(angle) * p->radius / 4096, p->heading.pad);
        size = rcos(angle) * 3 / 2;
        spin.z = p->heading.pad - (s16)p->timer * 30;
        g_FieldFxSpriteParams.parameter00 = 0x20;
        g_FieldFxSpriteParams.parameter02 = 2;
        g_FieldFxSpriteParams.extent_x = 0x20;
        g_FieldFxSpriteParams.extent_y = 0x20;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 1;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        FieldFx_SampleColorTrack(g_SceneE22TwistTrailColorTrack, &color, (s16)p->timer);
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        FieldFx_DrawShapeQuads(&offset, &spin, size, size, (s16)g_FieldFxSpriteParams.parameter02 + 100,
                      GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 10
                                                                      : palette + 6),
                      1, 0x80, &color);
        break;
    }
    }
    return 0;
}

/* Twist model controller: a model bolt that rides an actor joint along its
 * heading; it waits 16 frames, then for 16 frames advances and sheds a
 * twisting trail particle per frame. It draws as a spinning model, plus
 * glows and flares while it advances. */
int SceneE22_TwistModel(int mode, SceneE22TwistModel *bolt) {
    RenderColor color = g_SceneE22BoltColor;
    GteShortVector position;
    GteShortVector step;
    GteRotation rotation;
    GteRotation spin = g_SceneE22ZeroSpin;
    GteShortVector offset = g_SceneE22JointAnchor;
    RoomOrbitTrailParticle *child;
    int glow;
    int angle;
    int modelScale;

    switch (mode) {
    case 0:
        bolt->state = 0;
        bolt->timer = 0;
        bolt->reserved1C = 0;
        g_SceneE22BoltModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC54C4704);
        FieldFx_ModelScrollUv(g_SceneE22BoltModel, 0, 0);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, 0, &offset, &bolt->position);
        bolt->origin.x = bolt->position.x;
        bolt->origin.y = bolt->position.y;
        bolt->origin.z = bolt->position.z;
        FieldFx_GetTransformLookAngles((struct RoomFxTransformOwner *)ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool, 0,
                      &bolt->heading);
        bolt->heading.x -= 0x220;
        bolt->heading.y -= 0x80;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x18,
                             (FieldFxCallbackListCallback)SceneE22_TwistTrailCallback);
    case 1:
        FieldFx_AnglesToVector(&bolt->heading, 0x140, &position);
        position.x += bolt->position.x;
        position.y += bolt->position.y;
        position.z += bolt->position.z;
        switch (bolt->state) {
        case 0:
            bolt->timer++;
            if (bolt->timer < 0x10) break;
            bolt->state = 1;
            bolt->timer = 0;
            break;
        case 1:
            bolt->timer++;
            FieldFx_AnglesToVector(&bolt->heading, 0xA0, &step);
            bolt->position.x += step.x;
            bolt->position.y += step.y;
            bolt->position.z += step.z;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                child->radius = (rand() & 0xFF) + 0x180;
                child->heading.x = bolt->heading.x;
                child->heading.y = bolt->heading.y;
                child->heading.z = bolt->heading.z;
                child->heading.pad = rand();
                child->state = 0;
                child->timer = 0;
            }
            if (bolt->timer < 0x10) break;
            bolt->state = 2;
            bolt->timer = 0;
            break;
        case 2:
            return 2;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_AnglesToVector(&bolt->heading, 0x140, &position);
        position.x += bolt->position.x;
        position.y += bolt->position.y;
        position.z += bolt->position.z;
        modelScale = 0x2AA;
        switch (bolt->state) {
        case 0: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            glow = rsin(bolt->timer << 6) / 32;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11);
                int tpage = g_FieldFxTpageTable[*slot];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
                page = (u16)(g_FieldFxTpageTable[*slot] | GetTPage(0, 1, 0, 0));
                kind = D_800F2E2C;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_SceneE22BoltModel);
            FieldFx_ModelScaleColors(g_SceneE22BoltModel, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22BoltModel, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22BoltModel);
            break;
        }
        case 1: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            angle = bolt->timer << 6;
            glow = (rcos(angle) / 2 + 0x800) / 32;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11);
                int tpage = g_FieldFxTpageTable[*slot];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
                page = (u16)(g_FieldFxTpageTable[*slot] | GetTPage(0, 1, 0, 0));
                kind = D_800F2E2C;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_SceneE22BoltModel);
            FieldFx_ModelScaleColors(g_SceneE22BoltModel, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22BoltModel, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22BoltModel);
            {
                int kind;
                int palette;
                g_FieldFxSpriteParams.parameter00 = 0x40;
                g_FieldFxSpriteParams.parameter02 = 4;
                g_FieldFxSpriteParams.extent_x = 0x40;
                g_FieldFxSpriteParams.extent_y = 0x40;
                {
                    int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot11)];
                    g_FieldFxSpriteParams.palette = 3;
                    g_FieldFxSpriteParams.parameter06 = 1;
                    g_FieldFxSpriteParams.parameter0A = 0;
                    g_FieldFxSpriteParams.depth = 0x28;
                    g_FieldFxSpriteParams.tpage = tpage;
                }
                spin.z = bolt->timer * 32;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&position, &spin, 0x1000, 0x1000, 4,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3),
                              1, glow / 2, 0);
            }
            modelScale = rsin(angle) + 0x1000;
            FieldFx_DrawStarFan(&position, 0x44C, 0x5DC, 0x20, 0, modelScale, modelScale, &color, 0,
                          glow, 1);
            {
                int kind;
                int palette;
                g_FieldFxSpriteParams.parameter00 = 0x20;
                g_FieldFxSpriteParams.parameter02 = 2;
                g_FieldFxSpriteParams.extent_x = 0x20;
                g_FieldFxSpriteParams.extent_y = 0x20;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 8;
                else palette += 4;
                FieldFx_DrawPulledSprite(&position, 0x1000, 0x1000, (s16)g_FieldFxSpriteParams.parameter02 + 0x64,
                              GetClut(0, palette), 1, glow, 0, 0x600);
            }
            {
                int kind;
                int palette;
                {
                    int tpage = g_FieldFxTpageTable[TEXTURE_SLOT_RECORD(g_FieldFxTpageSlot3)];
                    g_FieldFxSpriteParams.palette = 3;
                    g_FieldFxSpriteParams.parameter06 = 0;
                    g_FieldFxSpriteParams.parameter00 = 0x20;
                    g_FieldFxSpriteParams.parameter02 = 2;
                    g_FieldFxSpriteParams.extent_x = 0x20;
                    g_FieldFxSpriteParams.extent_y = 0x20;
                    g_FieldFxSpriteParams.tpage = tpage;
                }
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                FieldFx_DrawPulledSprite(&position, 0x1800, 0x1800, 2,
                              GetClut(0x40, palette), 1, glow, 0, 0xA00);
            }
            {
                int kind;
                int palette;
                g_FieldFxSpriteParams.parameter00 = 0x40;
                g_FieldFxSpriteParams.parameter02 = 4;
                g_FieldFxSpriteParams.extent_x = 0x40;
                g_FieldFxSpriteParams.extent_y = 0x40;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
                FieldFx_DrawPulledSprite(&position, 0x1800, 0x1800, 4,
                              GetClut(0x50, palette), 1, glow, 0, 0xE00);
            }
            break;
        }
        case 2:
            break;
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

/* Twist beam particle: a twisting trail glow (state 0), a flickering beam
 * held until the release flag is set (state 1), or a swelling ring that
 * drifts along its heading (state 2). */
int SceneE22_TwistBeamCallback(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = g_SceneE22ZeroSpin;
    GteShortVector offset;
    RenderColor color;
    RenderColor colorA = g_SceneE22TwistBeamColorA;
    RenderColor colorB = g_SceneE22TwistBeamColorB;
    int angle;
    int size;
    int alpha;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->heading.pad += 0x20;
            if ((s16)p->timer < 0x20) break;
            return 1;
        case 1:
            p->timer++;
            if (g_SceneE22TwistBeamStop == 0) break;
            return 1;
        case 2:
            p->timer++;
            p->heading.x += 8;
            p->heading.y += 0xC;
            if ((s16)p->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)p->timer << 5;
            FieldFx_OffsetPointByAngles(p, &offset, 200, &p->heading,
                          rsin(angle) * p->radius / 4096, p->heading.pad);
            size = rcos(angle) * 3 / 2;
            spin.z = p->heading.pad - (s16)p->timer * 30;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_SampleColorTrack(g_SceneE22TwistBeamColorTrack, &color, (s16)p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&offset, &spin, size, size, (s16)g_FieldFxSpriteParams.parameter02 + 100,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 10
                                                                          : palette + 6),
                          1, 0x80, &color);
            return 0;
        }
        case 1: {
            int u;
            int v;
            alpha = 0x96;
            if (p->timer & 1) alpha = 0x80;
            u = ((p->heading.y & 1) << 6) + 0x80;
            v = 0xA0;
            *(u32 *)&color = 0x808080;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot3];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            FieldFx_DrawRotatedQuad((GteShortVector *)p, (GteRotation *)&p->heading, p->radius,
                          0xBE, u, v, 0x40, 0x20,
                          GetClut(0x20, g_FieldFxPaletteEntry3), &color, 0, alpha, 1);
            return 0;
        }
        case 2:
            size = rcos((s16)p->timer << 6) / 8;
            {
                RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            FieldFx_DrawRotatedTriangle((GteShortVector *)p, (GteRotation *)&p->heading, 0x1004, size, &colorA, &colorB,
                          &colorB, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}

/* Sweep bolt controller: the bolt swings out from its origin over sixteen
 * frames shedding twist beam particles, then hovers for 24 frames shaking
 * the screen and spraying sparks. It draws as a spinning model with a glow,
 * a halo and a ring. */
int SceneE22_SweepBolt(int mode, SceneE22SweepBolt *bolt, GteShortVector *origin) {
    RenderColor color = g_SceneE22BoltColor;
    RenderColor haloColor = g_SceneE22SweepBoltHaloColor;
    GteShortVector tip;
    GteRotation rotation;
    GteRotation spin = g_SceneE22ZeroSpin;
    GteRotation tilt = g_SceneE22FlatTilt;
    RoomOrbitTrailParticle *child;
    int glow;
    int angle;
    int modelScale;
    int i;

    switch (mode) {
    case 0:
        bolt->state = 0;
        bolt->timer = 0;
        bolt->reserved1C = 0;
        bolt->sweep = 0;
        g_SceneE22TwistBeamStop = 0;
        g_SceneE22BoltModel = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC54C4704);
        FieldFx_ModelScrollUv(g_SceneE22BoltModel, 0, 0);
        if (FX_TASK_AS(SceneE22EmberEvent)->active) {
            SceneE22EmberSlot *pool = ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool;
            if (pool) {
                SceneE22EmberObject *object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        bolt->origin.x = origin->x;
        bolt->origin.y = origin->y;
        bolt->origin.z = origin->z;
        bolt->heading.x = -0x320;
        bolt->heading.y = -0x200;
        bolt->heading.z = 0;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x20,
                             (FieldFxCallbackListCallback)SceneE22_TwistBeamCallback);
    case 1:
        FieldFx_AnglesToVector(&bolt->heading, -((0x10 - bolt->sweep) * 320), &bolt->position);
        bolt->position.x += bolt->origin.x;
        bolt->position.y += bolt->origin.y;
        bolt->position.z += bolt->origin.z;
        FieldFx_AnglesToVector(&bolt->heading, 0x140, &tip);
        tip.x += bolt->position.x;
        tip.y += bolt->position.y;
        tip.z += bolt->position.z;
        switch (bolt->state) {
        case 0:
            bolt->timer++;
            bolt->sweep++;
            child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (child) {
                child->x = tip.x;
                child->y = tip.y;
                child->z = tip.z;
                child->radius = (rand() & 0xFF) + 0x180;
                child->heading.x = bolt->heading.x;
                child->heading.y = bolt->heading.y;
                child->heading.z = bolt->heading.z;
                child->heading.pad = rand();
                child->state = 0;
                child->timer = 0;
            }
            if (bolt->timer < 0x10) break;
            bolt->state = 1;
            bolt->timer = 0;
            if (g_LoadedSceneAssetBlock[0]) {
                Akao_SendTableCommand(g_LoadedSceneAssetBlock[0], 0x5D8, FieldFx_GetOwnerProgress(), 0x80, 0x7F);
                if (g_LoadedSceneAssetBlock[0]) {
                    Akao_SendTableCommand(g_LoadedSceneAssetBlock[0], 0x5D9, 0x80, 0x80, 0x7F);
                }
            }
            break;
        case 1:
            bolt->timer++;
            angle = (bolt->timer << 10) / 24;
            Battle_HaltOnPositiveX();
            if (bolt->timer < 9) Geo_ClipAlternatingArcPoint(2, 8, bolt->timer);
            bolt->position.x = bolt->origin.x;
            bolt->position.y = bolt->origin.y;
            bolt->position.z = bolt->origin.z;
            if (g_FieldFxCurrentAge & 1) bolt->position.y += rcos(angle) / 512;
            else bolt->position.y -= rcos(angle) / 512;
            if (bolt->timer < 0xD) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = bolt->origin.x;
                    child->y = bolt->origin.y;
                    child->z = bolt->origin.z;
                    child->radius = (rand() & 0x3FF) + 0x7D0;
                    child->heading.x = 0;
                    child->heading.y = rand();
                    child->heading.z = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if ((u16)(bolt->timer - 4) < 5) {
                for (i = 0; i < 2; i++) {
                    child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                    if (child) {
                        child->x = bolt->origin.x;
                        child->y = bolt->origin.y;
                        child->z = bolt->origin.z;
                        child->heading.x = rand() % 768 + 0x100;
                        child->heading.y = rand();
                        child->heading.z = rand();
                        child->state = 2;
                        child->timer = 0;
                    }
                }
            }
            if (bolt->timer < 0x18) break;
            Battle_InitEntityColors();
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_AnglesToVector(&bolt->heading, 0x140, &tip);
        tip.x += bolt->position.x;
        tip.y += bolt->position.y;
        tip.z += bolt->position.z;
        modelScale = 0x2AA;
        switch (bolt->state) {
        case 0: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            angle = bolt->timer << 6;
            glow = (rcos(angle) / 2 + 0x800) / 32;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &g_FieldFxTpageSlot11;
                int tpage = g_FieldFxTpageTable[*slot];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
                page = (u16)(g_FieldFxTpageTable[*slot] | GetTPage(0, 1, 0, 0));
                kind = D_800F2E2C;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_SceneE22BoltModel);
            FieldFx_ModelScaleColors(g_SceneE22BoltModel, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22BoltModel, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22BoltModel);
            {
                int kind;
                int palette;
                g_FieldFxSpriteParams.parameter00 = 0x40;
                g_FieldFxSpriteParams.parameter02 = 4;
                g_FieldFxSpriteParams.extent_x = 0x40;
                g_FieldFxSpriteParams.extent_y = 0x40;
                {
                    int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlot11];
                    g_FieldFxSpriteParams.palette = 3;
                    g_FieldFxSpriteParams.parameter06 = 1;
                    g_FieldFxSpriteParams.parameter0A = 0;
                    g_FieldFxSpriteParams.depth = 0x28;
                    g_FieldFxSpriteParams.tpage = tpage;
                }
                spin.z = bolt->timer * 32;
                kind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_DrawShapeQuads(&tip, &spin, 0x1000, 0x1000, 4,
                              GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3),
                              1, glow / 2, 0);
            }
            modelScale = rsin(angle) + 0x1000;
            FieldFx_DrawStarFan(&tip, 0x44C, 0x5DC, 0x20, 0, modelScale, modelScale, &color, 0,
                          glow, 1);
            break;
        }
        case 1: {
            SceneE22ModelMatrix matrix;
            SceneE22ModelScale scale;
            angle = (bolt->timer << 10) / 24;
            glow = rcos(angle) / 32;
            if (g_FieldFxCurrentAge & 1)
                glow = glow * 3 / 4;
            {
                int kind;
                int palette;
                int page;
                u16 *slot = &g_FieldFxTpageSlot11;
                int tpage = g_FieldFxTpageTable[*slot];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
                page = (u16)(g_FieldFxTpageTable[*slot] | GetTPage(0, 1, 0, 0));
                kind = D_800F2E2C;
                palette = g_FieldFxPaletteTable[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 7
                                                                             : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            rotation.x = bolt->heading.x;
            rotation.y = bolt->heading.y;
            rotation.z = bolt->heading.z;
            rotation.x += 0x800;
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = bolt->position.x;
            matrix.t[1] = bolt->position.y;
            matrix.t[2] = bolt->position.z;
            scale.x = modelScale;
            scale.y = modelScale;
            scale.z = modelScale;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(g_SceneE22BoltModel);
            FieldFx_ModelScaleColors(g_SceneE22BoltModel, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22BoltModel, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22BoltModel);
            modelScale = rcos(angle) + 0x1000;
            spin.z = bolt->timer * 32;
            FieldFx_DrawStarFan(&bolt->origin, 1000, 0x32, 8, &spin, modelScale, modelScale,
                          &haloColor, 0, glow, 1);
            modelScale = rsin(angle);
            tilt.z = -(bolt->timer * 32);
            FieldFx_DrawRingBand(&bolt->origin, 0x7D0, 0xFA0, 0x10, &tilt, modelScale, modelScale, 0,
                          &color, glow, 1);
            break;
        }
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 4;
        break;
    }
    return 0;
}

int SceneE22_QuakeSparkCallback(int mode, SceneE22Effect *effect) {
    GteRotation position = g_SceneE22ZeroSpin;
    int angle, sine, brightness, palette, kind;
    u16 clut;

    switch (mode) {
    case 1: {
        if (effect->state != 0) return 0;
        effect->frame++;
        effect->y -= (effect->speed * 24) / 4096;
        if (effect->frame >= 32) return 1;
        break;
    }
    case 2: {
        if (effect->state != 0) return 0;
        angle = effect->frame << 5;
        position.z = g_FieldFxCurrentAge << 5;
        sine = rcos(angle);
        brightness = (effect->speed * sine) / 4096;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == 4 && g_FieldFxAltPaletteFlag) palette += 4;
        clut = GetClut(112, palette);
        FieldFx_DrawShapeQuads((GteShortVector *)effect, &position, brightness, brightness, 0x22, clut, 255, 128, 0);
        break;
    }
    }
    return 0;
}

/* Quake blast controller: on frame 6 it lights the actor if it stands in
 * reach, sprays sparks for six frames and ends after 40. It flashes the
 * screen, then draws a floor glow, a halo, a ring and three shock models
 * that spread from the blast. */
int SceneE22_QuakeBlast(int mode, SceneE22QuakeBlast *fx, SceneE22QuakeTarget *target) {
    GteShortVector position;
    GteRotation tilt = g_SceneE22FlatTilt;
    GteRotation rotation;
    SceneE22QuakeColor color;
    RenderColor flash = g_SceneE22QuakeFlashColor;
    GteShortVector floor;
    GteRotation flat;
    RoomOrbitTrailParticle *child;
    SceneE22QuakeObjectChannel *channel;
    int glow;
    int phase;
    int scale;
    int i;

    switch (mode) {
    case 0:
        fx->state = 0;
        fx->timer = 0;
        if (g_LoadedSceneAssetBlock[0]) {
            Akao_SendTableCommand(g_LoadedSceneAssetBlock[0], 0x5DA, FieldFx_GetOwnerProgress(), 0x80, 0x7F);
        }
        g_SceneE22QuakeModel0 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC58C4704);
        FieldFx_ModelScrollUv(g_SceneE22QuakeModel0, 0, 0);
        g_SceneE22QuakeModel1 = Asset_FindTable08ByU32Key(g_LoadedSceneAssetBlock[0], 0xC5CC4704);
        FieldFx_ModelScrollUv(g_SceneE22QuakeModel1, 0, 0);
        fx->position.x = target->x;
        fx->position.y = target->y;
        fx->position.z = target->z;
        return FieldFx_InitCallbackList(ANIM_POOL_AS(SceneE22QuakeChannel)->pool, 0xC, 0x20,
                             (FieldFxCallbackListCallback)SceneE22_QuakeSparkCallback);
    case 1:
        fx->timer++;
        if (fx->timer == 6 && FieldFx_IsPlayerWithinRadius(fx, target->radius) && FX_TASK_AS(SceneE22QuakeEvent)->active) {
            channel = ANIM_OWNER_AS(SceneE22QuakeObjectChannel);
            if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                g_PlayerEntity->state->flags |= 0x4000;
                channel->pool->object->flags =
                    (channel->pool->object->flags & 0xC0FFFFFF) | 0x21000000;
                channel->pool->object->flags |= 0x80000000;
            }
        }
        if (fx->timer < 7) {
            phase = 0x1000;
            for (i = 0; i < 4; i++) {
                child = FieldFx_AcquireCallbackSlot(ANIM_POOL_AS(SceneE22QuakeChannel)->pool);
                if (child) {
                    child->x = fx->position.x;
                    child->y = fx->position.y;
                    child->z = fx->position.z;
                    child->radius = (rand() & 0xFFF) + 0x800;
                    child->y -= rand() & 0x7F;
                    child->x += rand() % phase - phase / 2;
                    child->z += rand() % phase - phase / 2;
                    child->heading.x = 0;
                    child->heading.y = 0;
                }
            }
        }
        if (fx->timer < 0x28) break;
        return 1;
    case 2:
        position.x = fx->position.x;
        position.y = fx->position.y;
        position.z = fx->position.z;
        {
            RenderMatrixSlot *matrixSlot = &g_ViewMatrixSlot;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = 0x29;
        if (fx->timer < 4) {
            color.word = 0xFFFFFF;
            FieldFx_DrawScreenTint((unsigned char *)&color.color, 0x6E, 2, 8);
        } else if (fx->timer < 6) {
            FieldFx_DrawScreenTint((unsigned char *)&flash, 0x80, 1, 8);
        } else if (fx->timer < 0xD) {
            FieldFx_DrawScreenTint((unsigned char *)&flash, rcos(((fx->timer - 6) << 10) / 6) / 64, 1, 8);
        }
        glow = rcos((fx->timer << 10) / 40) / 32;
        if ((u16)fx->timer & 1)
            glow = glow * 3 / 4;
        {
            int kind;
            int palette;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.flags = 1;
            floor.x = position.x;
            floor.z = position.z;
            floor.y = g_RoomFloorY->raw;
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            FieldFx_DrawShapeQuads(&floor, &flat, 0x2000, 0x2000, 0x64,
                          GetClut(0, (kind == 4 && g_FieldFxAltPaletteFlag != 0) ? palette + 8
                                                                         : palette + 4),
                          1, glow, 0);
        }
        FieldFx_SampleColorTrack(g_SceneE22QuakeColorTrack, &color.color, fx->timer);
        FieldFx_DrawStarFan(&position, 0x4B0, 0x5DC, 0x10, 0, 0x1000, 0x1000, &color.color, 0,
                      glow, 1);
        phase = (fx->timer << 10) / 40;
        scale = rsin(phase);
        FieldFx_DrawRingBand(&position, 0xBB8, 0x1770, 0x12, &tilt, scale, scale, 0, &color.color,
                      glow, 1);
        g_FieldFxSpriteParams.depth = 4;
        glow = rcos(phase) / 32;
        scale = rsin(phase) + 0x800;
        {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            rotation.x = 0;
            rotation.y = rsin(phase);
            rotation.z = 0;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            page = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlots[3]] | GetTPage(0, 1, 0, 0));
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x10, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            shape.x = scale;
            shape.y = scale * 2;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            ScaleMatrix(&matrix, &shape);
            FieldFx_ModelSaveColors(g_SceneE22QuakeModel0);
            FieldFx_ModelScaleColors(g_SceneE22QuakeModel0, (u16)(glow * 3 / 2));
            FieldFx_ModelDraw(g_SceneE22QuakeModel0, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22QuakeModel0);
        }
        scale = scale / 2;
        {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            rotation.x = 0;
            rotation.y = rsin(phase) * 2;
            rotation.z = 0;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            page = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlots[3]] | GetTPage(0, 1, 0, 0));
            kind = D_800F2E2C;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x10, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            shape.x = scale;
            shape.y = scale * 8;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            ScaleMatrix(&matrix, &shape);
            FieldFx_ModelSaveColors(g_SceneE22QuakeModel0);
            FieldFx_ModelScaleColors(g_SceneE22QuakeModel0, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22QuakeModel0, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22QuakeModel0);
        }
        if (fx->timer < 0x19) {
            SceneE22QuakeMatrix matrix;
            SceneE22QuakeScale shape;
            int kind;
            int palette;
            int page;
            phase = (fx->timer << 10) / 24;
            glow = rcos(phase) / 32;
            scale = rcos(phase);
            rotation.x = 0;
            rotation.y = fx->timer * 40;
            rotation.z = 0;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            page = (u16)(g_FieldFxTpageTable[g_FieldFxTpageSlots[3]] | GetTPage(0, 1, 0, 0));
            kind = D_800F2E2C;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x30, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            shape.x = scale;
            shape.y = 0x1000;
            shape.z = scale;
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            ScaleMatrix(&matrix, &shape);
            FieldFx_ModelSaveColors(g_SceneE22QuakeModel1);
            FieldFx_ModelScaleColors(g_SceneE22QuakeModel1, (u16)glow);
            FieldFx_ModelDraw(g_SceneE22QuakeModel1, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(g_SceneE22QuakeModel1);
        }
        g_FieldFxSpriteParams.parameter00 = 0x10;
        g_FieldFxSpriteParams.parameter02 = 1;
        g_FieldFxSpriteParams.extent_x = 0x10;
        g_FieldFxSpriteParams.extent_y = 0x10;
        {
            int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
            g_FieldFxSpriteParams.palette = 3;
            g_FieldFxSpriteParams.parameter06 = 0;
            g_FieldFxSpriteParams.parameter0A = 0;
            g_FieldFxSpriteParams.tpage = tpage;
        }
        break;
    }
    return 0;
}

int *SceneE22_GetBurstAnchor(void) {
    return (int *)&g_SceneE22BurstAnchor;
}

int SceneE22_PlayerGate(int mode) {
    switch (mode) {
    case 0:
        FX_TASK_AS(WaitState)->complete = 0;
        FX_TASK_AS(WaitState)->signaled = 0;
        g_PlayerEntity->state->flags &= ~0x10000000u;
        return 0;
    case 1:
        if (g_PlayerEntity->state->flags & 0x10000000u) {
            FX_TASK_AS(WaitState)->complete = 1;
            FX_TASK_AS(WaitState)->signaled = 1;
            return 1;
        }
        if (g_FieldFxCurrentAge >= 100) {
            FX_TASK_AS(WaitState)->complete = 1;
            FX_TASK_AS(WaitState)->signaled = 0;
            return 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}

int *SceneE22_EmberDriftConfigure(void) {
    return (int *)&g_SceneE22BurstAnchor;
}

int *SceneE22_EmberColumnConfigure(void) {
    return (int *)&g_SceneE22BurstAnchor;
}

s16 *SceneE22_ConfigureBlastTarget(int mode, int value, int unused, int extra)
{
    u16 saved;
    char *actor;
    s16 *out;

    if (mode == 1) {
        saved = g_RoomFloorY->raw;
        actor = (char *)g_PlayerEntity;
        out = (s16 *)&g_SceneE22BlastTarget;
        *out = value;
        g_SceneE22BlastTarget.z = extra;
        g_SceneE22BlastTarget.y = saved;
        FieldFx_GetActorPosition(actor, 1, out);
    } else {
        if (value <= 0)
            value = 1;
        g_SceneE22BlastTarget.radius = value;
    }

    return (s16 *)&g_SceneE22BlastTarget;
}
