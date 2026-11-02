#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/battle.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_actor.h"
#include "pe1/field_billboard.h"
#include "pe1/field_effect_modules.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/fieldfx_particle.h"
#include "pe1/fieldfx_slot.h"
#include "pe1/fieldfx_state.h"
#include "pe1/fieldfx_task.h"
#include "pe1/fixed_point.h"
#include "pe1/game_audio_state.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_libc.h"
#include "pe1/random.h"

int PointSprite_PlantScript(char *object) {
    FieldActor *actor;
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = g_PointSpriteScript;
    actor = ((FieldFxObjectPrefix *)object)->actor;

    g_PointSpriteActorPosition.x = actor->pos_x >> 16;
    g_PointSpriteActorPosition.y = actor->pos_y >> 16;
    g_PointSpriteActorPosition.z = actor->pos_z >> 16;
    g_PointSpriteOwner = actor;

    g_PointSpriteStreakSprite.oriented.rotation.x = 0;
    g_PointSpriteStreakSprite.oriented.rotation.y = 0;
    g_PointSpriteStreakSprite.oriented.rotation.z = 0;
    g_PointSpriteStreakSprite.oriented.cell = 0x68;
    g_PointSpriteStreakSprite.oriented.clut = 0;
    g_PointSpriteStreakSprite.oriented.depth = -100;

    g_PointSpriteState0.oriented.rotation.x = 0x400;
    g_PointSpriteState0.oriented.rotation.y = 0;
    g_PointSpriteState0.oriented.rotation.z = 0;
    g_PointSpriteState0.oriented.cell = 0x68;
    g_PointSpriteState0.oriented.clut = 0;
    g_PointSpriteState0.oriented.depth = 0;

    g_PointSpriteGroundRingSprite.oriented.rotation.x = 0x400;
    g_PointSpriteGroundRingSprite.oriented.rotation.y = 0;
    g_PointSpriteGroundRingSprite.oriented.rotation.z = 0;
    g_PointSpriteGroundRingSprite.oriented.cell = 0x68;
    g_PointSpriteGroundRingSprite.oriented.clut = 0;
    g_PointSpriteGroundRingSprite.oriented.depth = -100;
    g_PointSpriteGroundRingSprite.oriented.rgb[0] = 0xA0;
    g_PointSpriteGroundRingSprite.oriented.rgb[1] = 0xA0;
    g_PointSpriteGroundRingSprite.oriented.rgb[2] = 0xA0;

    g_PointSpriteState4.oriented.rotation.x = 0x400;
    g_PointSpriteState4.oriented.rotation.y = 0;
    g_PointSpriteState4.oriented.rotation.z = 0;
    g_PointSpriteState4.oriented.cell = 0x46;
    g_PointSpriteState4.oriented.clut = 0x30;
    g_PointSpriteState4.oriented.depth = -0x50;
    g_PointSpriteState4.oriented.rgb[0] = 0x80;
    g_PointSpriteState4.oriented.rgb[1] = 0x80;
    g_PointSpriteState4.oriented.rgb[2] = 0x80;

    g_PointSpriteState1.oriented.scale.x = 0x1A8;
    g_PointSpriteState1.oriented.scale.y = 0x1A8;
    g_PointSpriteState1.oriented.scale.z = 0x1A8;
    g_PointSpriteState1.oriented.rotation.x = 0;
    g_PointSpriteState1.oriented.rotation.y = 0;
    g_PointSpriteState1.oriented.rotation.z = 0;
    g_PointSpriteState1.oriented.cell = 0xBC;
    g_PointSpriteState1.oriented.clut = 8;
    g_PointSpriteState1.oriented.depth = 0;
    g_PointSpriteState1.oriented.rgb[0] = 0x80;
    g_PointSpriteState1.oriented.rgb[1] = 0x80;
    g_PointSpriteState1.oriented.rgb[2] = 0x80;

    g_PointSpriteState3.oriented.rotation.x = 0x400;
    g_PointSpriteState3.oriented.rotation.y = 0;
    g_PointSpriteState3.oriented.rotation.z = 0;
    g_PointSpriteState3.oriented.cell = 0x42;
    g_PointSpriteState3.oriented.clut = 0x20;
    g_PointSpriteState3.oriented.depth = 0x32;
    g_PointSpriteState3.oriented.rgb[0] = 0x80;
    g_PointSpriteState3.oriented.rgb[1] = 0x80;
    g_PointSpriteState3.oriented.rgb[2] = 0x80;

    return 0;
}

int PointSprite_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int PointSprite_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_PointSpriteDrawList);
    return 0;
}

int PointSprite_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_PointSpriteUpdateList);
    second = FieldFx_RunScript(obj, g_PointSpriteInitList, g_PointSpriteSlotSizes);
    status = first | second;
    if (status == -1) {
        PointSprite_CloseTarget(obj);
    }
    return 0;
}

int PointSprite_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int PointSprite_ClassNop6(void) {
    return 0;
}

int PointSprite_ClassNop0(void) {
    return 0;
}

int PointSprite_InitRisingStreak(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    int x_base_a1;
    int z_base_a1;
    int x_rand;
    int z_rand;

    g_PointSpriteStreakSprite.oriented.rgb[0] = *FieldFx_RegPtr(0);
    g_PointSpriteStreakSprite.oriented.rgb[1] = *FieldFx_RegPtr(1);
    g_PointSpriteStreakSprite.oriented.rgb[2] = *FieldFx_RegPtr(2);

    x_rand = rand();
    x_base_a1 = g_PointSpriteActorPosition.x - 0x64;
    anim->point.x = x_base_a1 + (x_rand % 201);
    anim->point.y = g_PointSpriteActorPosition.y;

    z_rand = rand();
    z_base_a1 = g_PointSpriteActorPosition.z - 0x64;
    anim->extent = 0x20C;
    anim->scale = 0x7F;
    anim->point.z = z_base_a1 + (z_rand % 201);
}

void FieldFx_InitRadialEntries(void *arg0, void *arg1, FieldFxRadialParticles *anim) {
    int i;
    int angle;
    int lift;

    g_PointSpriteState0.oriented.rgb[0] = *FieldFx_RegPtr(3);
    g_PointSpriteState0.oriented.rgb[1] = *FieldFx_RegPtr(4);
    g_PointSpriteState0.oriented.rgb[2] = *FieldFx_RegPtr(5);

    anim->scale = 0x7F;
    if (((Combatant *)D_8009CD10->core)->action->actionCode.actionId == 3) {
        anim->count = 8;
    } else {
        anim->count = 0x10;
    }

    for (i = 0; i < anim->count; i++) {
        anim->points[i].position.x = g_PointSpriteActorPosition.x;
        anim->points[i].position.y = g_PointSpriteActorPosition.y;
        anim->points[i].position.z = g_PointSpriteActorPosition.z;

        if (anim->count == 0x10) {
            angle = i << 8;
        } else {
            angle = i << 9;
        }

        lift = -0x1400;
        anim->velocity[i].x = rsin(angle);
        anim->velocity[i].y = lift;
        anim->velocity[i].z = rcos(angle);
        anim->extent[i] = 0x258;
    }
}

int PointSprite_InitGroundRing(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 z;

    anim->point.x = g_PointSpriteActorPosition.x;
    anim->point.y = g_PointSpriteActorPosition.y;
    z = g_PointSpriteActorPosition.z;
    anim->extent = 0xBE8;
    anim->scale = 0x7F;
    anim->point.z = z;
}

int PointSprite_InitBillboard(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 z;

    anim->point.x = g_PointSpriteActorPosition.x;
    anim->point.y = g_PointSpriteActorPosition.y - 0x64;
    z = g_PointSpriteActorPosition.z;
    anim->extent = 0;
    anim->scale = 0x7F;
    anim->point.z = z;
}

void FieldFx_InitScatteredParticles(void *arg0, void *arg1, FieldFxScatteredParticles *state)
{
    int i;

    state->points.scale = 127;
    if (((Combatant *)D_8009CD10->core)->action->actionCode.actionId == 3)
        state->points.count = 4;
    else
        state->points.count = 16;

    for (i = 0; i < state->points.count; i++) {
        state->points.x[i] = g_PointSpriteActorPosition.x;
        state->points.y[i] = g_PointSpriteActorPosition.y;
        state->points.z[i] = g_PointSpriteActorPosition.z;
        state->velocity_x[i] = (rand() % 11 - 5) * 256;
        state->velocity_y[i] = -(rand() % 16 + 20) * 256;
        state->velocity_z[i] = (rand() % 11 - 5) * 256;
    }
}

/* Point sprite setup, draw and update callbacks, including radial and
 * scattered particle arrays. Shared payloads are declared in fieldfx_task.h. */

int PointSprite_InitOrientedFlash(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 z;

    anim->point.x = g_PointSpriteActorPosition.x;
    anim->point.y = g_PointSpriteActorPosition.y;
    z = g_PointSpriteActorPosition.z;
    anim->extent = 0x224;
    anim->scale = 0x7F;
    anim->point.z = z;
}

int PointSprite_DrawRisingStreak(void *arg0, FieldFxSlot *params, FieldFxPointSprite *anim) {
    short *base_a0;
    FieldBillboard *output;
    FieldFxPointSprite *pointData = anim;
    u32 value_v0;
    int value_v1;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    value_v1 = params->counter;
    value_v0 = 0x80;
    value_v1 <<= 1;
    value_v0 -= value_v1;
    value_v1 = value_v0;
    value_v0 <<= 16;
    if (value_v0 > 0x7FFFFFFFU) {
        value_v1 = 0;
    }

    base_a0 = &g_PointSpriteStreakSprite.oriented.brightness;
    base_a0[0] = value_v1;
    value_v0 = pointData->point.x;
    output = (FieldBillboard *)((u8 *)base_a0 -
        PE1_OFFSETOF(FieldBillboard, brightness));
    output->position.x = value_v0;
    g_PointSpriteStreakSprite.oriented.position.y = pointData->point.y;
    g_PointSpriteStreakSprite.oriented.position.z = pointData->point.z;
    g_PointSpriteStreakSprite.oriented.scale.x = pointData->extent;
    g_PointSpriteStreakSprite.oriented.scale.y = pointData->extent * 2;
    g_PointSpriteStreakSprite.oriented.scale.z = pointData->extent;
    FieldFx_DrawBillboard(output);
}

void FieldFx_ProcessInterleavedPoints(void *arg0, void *arg1, FieldFxRadialParticles *anim) {
    unsigned int i;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    g_PointSpriteState0.oriented.scale.x = (s16)anim->extent[0];
    g_PointSpriteState0.oriented.scale.y = (s16)anim->extent[0];
    g_PointSpriteState0.oriented.scale.z = (s16)anim->extent[0];
    g_PointSpriteState0.billboard.brightness = anim->scale;

    for (i = 0; i < anim->count; i++) {
        g_PointSpriteState0.billboard.position.x = anim->points[i].position.x;
        g_PointSpriteState0.billboard.position.y = anim->points[i].position.y;
        g_PointSpriteState0.billboard.position.z = anim->points[i].position.z;
        FieldFx_DrawBillboard(&g_PointSpriteState0.billboard);
    }
}

int PointSprite_DrawGroundRing(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(3);

    value_v0 = anim->point.x;
    output = &g_PointSpriteGroundRingSprite.oriented;
    output->position.x = value_v0;
    g_PointSpriteGroundRingSprite.oriented.position.y = anim->point.y;
    g_PointSpriteGroundRingSprite.oriented.position.z = anim->point.z;
    g_PointSpriteGroundRingSprite.oriented.scale.x = anim->extent;
    g_PointSpriteGroundRingSprite.oriented.scale.y = anim->extent;
    g_PointSpriteGroundRingSprite.oriented.scale.z = anim->extent;
    g_PointSpriteGroundRingSprite.oriented.brightness = (signed char)anim->scale;
    FieldFx_DrawOrientedSprite(output);
}

int PointSprite_DrawBillboardFlash(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 value_v0;
    FieldBillboard *output;
    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    value_v0 = anim->point.x;
    output = &g_PointSpriteState4.billboard;
    output->position.x = value_v0;
    g_PointSpriteState4.oriented.position.y = anim->point.y;
    g_PointSpriteState4.oriented.position.z = anim->point.z;
    g_PointSpriteState4.oriented.scale.x = anim->extent;
    g_PointSpriteState4.oriented.scale.y = anim->extent;
    g_PointSpriteState4.oriented.scale.z = anim->extent;
    g_PointSpriteState4.oriented.brightness = (signed char)anim->scale;
    FieldFx_DrawBillboard(output);
}

void FieldFx_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldFxPointData *points) {
    unsigned int i;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x10, 0x10);
    FieldFx_SetSpriteBlendMode(0);

    g_PointSpriteState1.billboard.brightness = (points->scale << 24) >> 23;

    for (i = 0; i < points->count; i++) {
        g_PointSpriteState1.billboard.position.x = points->x[i];
        g_PointSpriteState1.billboard.position.y = points->y[i];
        g_PointSpriteState1.billboard.position.z = points->z[i];
        FieldFx_DrawBillboard(&g_PointSpriteState1.billboard);
    }
}

int PointSprite_DrawOrientedFlash(void *arg0, void *arg1, FieldFxPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    value_v0 = anim->point.x;
    output = &g_PointSpriteState3.oriented;
    output->position.x = value_v0;
    g_PointSpriteState3.oriented.position.y = anim->point.y;
    g_PointSpriteState3.oriented.position.z = anim->point.z;
    g_PointSpriteState3.oriented.scale.x = anim->extent;
    g_PointSpriteState3.oriented.scale.y = anim->extent;
    g_PointSpriteState3.oriented.scale.z = anim->extent;
    g_PointSpriteState3.oriented.brightness = (signed char)anim->scale;
    FieldFx_DrawOrientedSprite(output);
}

void PointSprite_UpdateRisingStreak(void *arg0, FieldFxSlot *state, FieldFxPointSprite *anim) {
    anim->point.x += 0; /* no horizontal drift, but the record is rewritten */
    anim->point.y -= 10;
    anim->scale -= 2;
    anim->extent += 0x1E;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

void PointSprite_UpdateRadialParticles(void *arg0, FieldFxSlot *state, FieldFxRadialParticles *anim) {
    int i;

    for (i = 0; i < anim->count; i++) {
        anim->points[i].position.x += anim->velocity[i].x >> 9;
        anim->points[i].position.y += anim->velocity[i].y >> 9;
        anim->points[i].position.z += anim->velocity[i].z >> 9;
        anim->velocity[i].y += 0xB4;
        anim->extent[i] += 0x18;
    }

    anim->scale -= 2;
    if (anim->scale < 2) {
        state->flag = 2;
    }
}

int PointSprite_UpdateGroundRing(void *arg0, FieldFxSlot *state, FieldFxPointSprite *anim) {
    anim->scale -= 2;
    anim->extent = (u16)anim->extent + 0x28;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

int PointSprite_UpdateBillboardFlash(void *arg0, FieldFxSlot *state, FieldFxPointSprite *anim) {
    anim->scale -= 6;
    anim->extent = (u16)anim->extent + 0xB4;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

void PointSprite_UpdateScatteredParticles(void *arg0, FieldFxSlot *state, FieldFxScatteredParticles *anim) {
    int i;

    for (i = 0; i < anim->points.count; i++) {
        anim->points.x[i] += anim->velocity_x[i] >> 8;
        anim->points.y[i] += anim->velocity_y[i] >> 8;
        anim->points.z[i] += anim->velocity_z[i] >> 8;
        anim->velocity_y[i] += 0xB4;
    }

    anim->points.scale -= 2;
    if ((s8)anim->points.scale < 2) {
        state->flag = 2;
    }
}

int PointSprite_UpdateOrientedFlash(void *arg0, FieldFxSlot *state, FieldFxPointSprite *anim) {
    anim->scale -= 8;
    anim->extent = (u16)anim->extent + 0x1A4;
    if ((s8)anim->scale < 0x14) {
        anim->scale = 0;
        state->flag = 2;
    }
}

static __inline__ void identity_matrix(GteMatrix *matrix)
{
    matrix->m[0][0] = matrix->m[1][1] = matrix->m[2][2] = 4096;
    matrix->m[0][1] = matrix->m[0][2] = matrix->m[1][0] =
        matrix->m[1][2] = matrix->m[2][0] = matrix->m[2][1] =
        matrix->t[0] = matrix->t[1] = matrix->t[2] = 0;
}

int BurstEffect_PlantScript(char *object)
{
    GteVector first, second, third;

    *FieldFx_GetSlot(object) = g_BurstEffectScript;
    first = g_BurstEffectScaleA;
    identity_matrix(&g_BurstEffectScaleMatrix0);
    ScaleMatrix(&g_BurstEffectScaleMatrix0, &first);
    second = g_BurstEffectScaleB;
    identity_matrix(&g_BurstEffectScaleMatrix1);
    ScaleMatrix(&g_BurstEffectScaleMatrix1, &second);
    third = g_BurstEffectScaleC;
    identity_matrix(&g_BurstEffectScaleMatrix2);
    ScaleMatrix(&g_BurstEffectScaleMatrix2, &third);

    g_BurstEffectSprite1.cell = 0x42;
    g_BurstEffectSprite1.clut = 0x20;
    g_BurstEffectSprite1.offset = 50;
    g_BurstEffectSprite1.depth = 127;
    g_BurstEffectSprite1.r = 128;
    g_BurstEffectSprite1.g = 128;
    g_BurstEffectSprite1.b = 128;

    g_BurstEffectSprite0.cell = 128;
    g_BurstEffectSprite0.clut = 4;
    g_BurstEffectSprite0.offset = -50;
    g_BurstEffectSprite0.depth = 127;
    g_BurstEffectSprite0.r = 128;
    g_BurstEffectSprite0.g = 128;
    g_BurstEffectSprite0.b = 128;

    g_BurstEffectSprite2.cell = 0x68;
    g_BurstEffectSprite2.clut = 0;
    g_BurstEffectSprite2.offset = -99;
    g_BurstEffectSprite2.depth = 127;
    g_BurstEffectSprite2.r = 64;
    g_BurstEffectSprite2.b = 64;
    g_BurstEffectSprite2.g = 128;

    g_BurstEffectSprite3.cell = 0x6E;
    g_BurstEffectSprite3.clut = 3;
    g_BurstEffectSprite3.offset = -100;
    g_BurstEffectSprite3.depth = 127;
    g_BurstEffectSprite3.r = 128;
    g_BurstEffectSprite3.g = 128;
    g_BurstEffectSprite3.b = 128;
    return 0;
}

int BurstEffect_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffect_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectDrawList);
    return 0;
}

int BurstEffect_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectInitList, g_BurstEffectSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffect_CloseTarget(obj);
    }
    return 0;
}

int BurstEffect_CloseTarget(u8 *state) {
    state[0] = 4;
    return 0;
}

int BurstEffect_ClassNop6(void) {
    return 0;
}

int BurstEffect_ClassNop0(void) {
    return 0;
}

int BurstEffect_InitTarget(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

    data = g_BurstEffectTarget;
    g_BurstEffectTargetPosition.x = data->render_object.target_x;
    g_BurstEffectTargetPosition.y = data->render_object.target_y;
    g_BurstEffectTargetPosition.z = data->render_object.target_z;

    if (g_AkaoRuntimeState.reset_pending != 0) {
        Akao_Cmd_24(g_FieldEffectSoundHeader, 0, 0x80, 0x7F);
    }

    FieldFx_EnsureEffectTexturePage(0);
}

int BurstEffect_InitGroundGlow(void *arg0, void *arg1, FieldFxBurstData *anim) {
    u16 x = g_BurstEffectTargetPosition.x;
    BattleEntity *entity = D_8009CD10;
    int y;
    u16 z;

    anim->header.origin_x = x;
    y = entity->posY.parts.integer;
    anim->header.origin_y = y;
    z = g_BurstEffectTargetPosition.z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x60C;
    anim->header.origin_z = z;
}

void FieldFx_InitRandomOffsetPairs(void *arg0, void *arg1,
                                     FieldFxBurstData *anim) {
    FieldFxPointTriple *base;
    u16 *base_values;
    FieldFxBurstWindow *window;
    u8 *entry;
    u16 z;
    int i;

    base = &g_BurstEffectTargetPosition;
    i = 0;

    anim->header.origin_x = base->x;
    anim->header.origin_y = base->y;
    z = base->z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x3B4;
    anim->header.shade = 0;
    anim->header.origin_z = z;
    base_values = &base->x;

    do {
        entry = &anim->bytes[(i & 0xFFFF) * 8];
        window = (FieldFxBurstWindow *)entry;
        window->point.x = base_values[0];
        window->point.y = base_values[1];
        window->point.z = base_values[2];
        window->offset.x = (rand() % 50) - 0x19;
        window->offset.y = (rand() % 50) - 0x19;
        i++;
        window->offset.z = 0;
    } while (((unsigned int)i & 0xFFFF) < 2);
}

int BurstEffect_InitSparkPoint(void *arg0, void *arg1, FieldFxBurstData *anim) {
    u16 z;

    anim->header.origin_x = (g_BurstEffectTargetPosition.x + (rand() % 201)) - 0x64;
    anim->header.origin_y = (g_BurstEffectTargetPosition.y + (rand() % 101)) - 0x32;
    z = g_BurstEffectTargetPosition.z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x544;
    anim->header.origin_z = z;
}

/* Two empty field effect callbacks. */
void BurstEffect_InitNop4(void) {
}

void BurstEffect_DrawNop0(void) {
}

int BurstEffect_DrawGroundGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 *field = &g_BurstEffectSprite1.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_BurstEffectSpriteRotation;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    *field = anim->brightness;
    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_BurstEffectSprite1, &matrix, 0);
}

int BurstEffect_DrawSparkPairs(void *arg0, void *arg1, FieldFxBurstData *anim) {
    FieldFxBurstWindow *window;
    u16 i;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    g_BurstEffectScaleMatrix1.t[0] = anim->header.origin_x;
    g_BurstEffectScaleMatrix1.t[1] = anim->header.origin_y;
    g_BurstEffectScaleMatrix1.t[2] = anim->header.origin_z;
    g_BurstEffectSprite0.cell = (anim->header.shade * 2) - 0x80;
    FieldFx_DrawGlowSprite(&g_BurstEffectSprite0, &g_BurstEffectScaleMatrix1, 1);

    FieldFx_SetSpriteBlendMode(3);

    for (i = 0; i < 2; i++) {
        window = (FieldFxBurstWindow *)&anim->bytes[i * 8];
        g_BurstEffectScaleMatrix2.t[0] = (s16)window->point.x;
        g_BurstEffectScaleMatrix2.t[1] = (s16)window->point.y;
        g_BurstEffectScaleMatrix2.t[2] = (s16)window->point.z;
        FieldFx_DrawGlowSprite(&g_BurstEffectSprite3, &g_BurstEffectScaleMatrix2, 1);
    }
}

/* Burst and spark effect setup, draw and lifecycle callbacks. */

int BurstEffect_DrawSparkPoint(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    int *base_a1;
    u16 *field_v1;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(3);

    base_a1 = &g_BurstEffectScaleMatrix0.t[0];
    field_v1 = &g_BurstEffectSprite2.depth;
    base_a1[0] = (s16)anim->position.x;
    g_BurstEffectScaleMatrix0.t[1] = (s16)anim->position.y;
    g_BurstEffectScaleMatrix0.t[2] = (s16)anim->position.z;
    *field_v1 = anim->brightness;
    FieldFx_DrawGlowSprite(&g_BurstEffectSprite2, &g_BurstEffectScaleMatrix0, 1);
}

void BurstEffect_DrawNop4(void) {
}

int BurstEffect_UpdateEnd(void *arg0, FieldFxSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int BurstEffect_UpdateGroundGlow(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 0xA;
    anim->scale += 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int BurstEffect_UpdateSparkPairs(void *arg0, FieldFxSlot *state, FieldFxTwoPointMotion *anim) {
    unsigned int i = 0;

    do {
        unsigned slot = i & 0xFFFF;

        i++;
        anim->position[slot].x = anim->position[slot].x + anim->velocity[slot].x;
        anim->position[slot].y = anim->position[slot].y + anim->velocity[slot].y;
        anim->position[slot].z = anim->position[slot].z + anim->velocity[slot].z;
    } while ((i & 0xFFFF) < 2);

    anim->age++;
    if ((signed char)anim->age >= 8) {
        state->flag = 2;
    }
}

int BurstEffect_UpdateSparkPoint(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldFxGlowPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 8;
    brightness -= 8;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 8;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (short)brightness) {
        state->flag = 2;
    }
}

int BurstEffect_UpdateEnd4(void *arg0, FieldFxSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int BurstEffectB_PlantScript(char *object) {
    void **slot = FieldFx_GetSlot(object);

    *slot = &g_BurstEffectBScript;

    g_BurstEffectBState2.oriented.scale.x = 0x5F4;
    g_BurstEffectBState2.oriented.scale.y = 0x5F4;
    g_BurstEffectBState2.oriented.scale.z = 0x5F4;
    g_BurstEffectBState2.oriented.rotation.x = 0;
    g_BurstEffectBState2.oriented.rotation.y = 0;
    g_BurstEffectBState2.oriented.rotation.z = 0;
    g_BurstEffectBState2.oriented.cell = 0x40;
    g_BurstEffectBState2.oriented.clut = 0x20;
    g_BurstEffectBState2.oriented.depth = -0x64;
    g_BurstEffectBState2.oriented.rgb[0] = 0x15;
    g_BurstEffectBState2.oriented.rgb[1] = 0x20;
    g_BurstEffectBState2.oriented.rgb[2] = 0x20;

    g_BurstEffectBState1.oriented.rotation.x = 0;
    g_BurstEffectBState1.oriented.rotation.y = 0;
    g_BurstEffectBState1.oriented.rotation.z = 0;
    g_BurstEffectBState1.oriented.cell = 0x46;
    g_BurstEffectBState1.oriented.clut = 0x30;
    g_BurstEffectBState1.oriented.depth = -0x6E;
    g_BurstEffectBState1.oriented.rgb[0] = 0xFF;
    g_BurstEffectBState1.oriented.rgb[1] = 0xB0;
    g_BurstEffectBState1.oriented.rgb[2] = 0xB0;

    g_BurstEffectBState0.oriented.scale.x = 0xA4;
    g_BurstEffectBState0.oriented.scale.y = 0xA4;
    g_BurstEffectBState0.oriented.scale.z = 0x108;
    g_BurstEffectBState0.oriented.rotation.x = 0;
    g_BurstEffectBState0.oriented.rotation.y = 0;
    g_BurstEffectBState0.oriented.rotation.z = 0;
    g_BurstEffectBState0.oriented.cell = 0x6E;
    g_BurstEffectBState0.oriented.clut = 3;
    g_BurstEffectBState0.oriented.depth = -0x96;
    g_BurstEffectBState0.oriented.rgb[0] = 0x80;
    g_BurstEffectBState0.oriented.rgb[1] = 0x80;
    g_BurstEffectBState0.oriented.rgb[2] = 0x80;

    return 0;
}

int BurstEffectB_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffectB_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectBDrawList);
    return 0;
}

int BurstEffectB_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectBUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectBInitList, g_BurstEffectBSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffectB_CloseTarget(obj);
    }
    return 0;
}

int BurstEffectB_CloseTarget(u8 *state) {
    state[0] = 4;
    return 0;
}

int BurstEffectB_ClassNop6(void) {
    return 0;
}

int BurstEffectB_ClassNop0(void) {
    return 0;
}

int BurstEffectB_InitTarget(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectBTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

    data = g_BurstEffectBTarget;
    g_BurstEffectBTargetPosition.x = data->render_object.target_x;
    g_BurstEffectBTargetPosition.y = data->render_object.target_y;
    g_BurstEffectBTargetPosition.z = data->render_object.target_z;

    if (g_AkaoRuntimeState.reset_pending != 0) {
        Akao_Cmd_24(g_FieldEffectSoundHeader, 0, 0x80, 0x7F);
    }

    FieldFx_EnsureEffectTexturePage(0);
}

int BurstEffectB_InitSparkCloud(void *arg0, void *arg1, FieldFxSparkCloud *anim) {
    u16 *base = &g_BurstEffectBTargetPosition.x;
    u16 *base_s4;
    int i = 0;
    int value;

    anim->center.position.x = base[0];
    anim->center.position.y = g_BurstEffectBTargetPosition.y;
    anim->center.position.z = g_BurstEffectBTargetPosition.z;
    anim->center.displacement.x = (rand() % 201) - 0x64;
    anim->center.displacement.y = (rand() % 101) - 0x32;

    base_s4 = base;
    anim->center.brightness = 0x7F;
    anim->center.displacement.z = 0;
    anim->center.scaleStep = 0;

    do {
        unsigned int slot = i & 0xFFFF;
        anim->position[0][slot] = base_s4[0];
        anim->position[1][slot] = base_s4[1];
        anim->position[2][slot] = base_s4[2];
        anim->velocity[0][slot] = (rand() % 50) - 0x19;
        value = (rand() % 50) - 0x19;
        i++;
        anim->velocity[2][slot] = 0;
        anim->velocity[1][slot] = value;
    } while ((unsigned int)(i & 0xFFFF) < 8U);
}

int BurstEffectB_InitSparkPoint(void *arg0, void *arg1, FieldFxSparkPoint *anim) {
    int value;

    anim->position.x = g_BurstEffectBTargetPosition.x;
    anim->position.y = g_BurstEffectBTargetPosition.y;
    anim->position.z = g_BurstEffectBTargetPosition.z;
    anim->brightness = 0x7F;
    anim->displacement.x = (rand() % 201) - 0x64;
    value = rand();
    anim->displacement.z = 0;
    anim->displacement.y = (value % 101) - 0x32;
}

void BurstEffectB_DrawNop(void) {
}

int BurstEffectB_DrawSparkCloud(void *arg0, void *arg1, FieldFxSparkCloud *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;
    unsigned int i;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    lhs_v0 = anim->center.position.x;
    rhs_v1 = anim->center.displacement.x;
    output = &g_BurstEffectBState1.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    g_BurstEffectBState1.oriented.position.y = anim->center.position.y + anim->center.displacement.y;
    g_BurstEffectBState1.oriented.position.z = anim->center.position.z + anim->center.displacement.z;
    g_BurstEffectBState1.oriented.scale.x = ((s8)anim->center.scaleStep * 8) + 0x20C;
    g_BurstEffectBState1.oriented.scale.y = ((s8)anim->center.scaleStep * 8) + 0x20C;
    g_BurstEffectBState1.oriented.scale.z = ((s8)anim->center.scaleStep * 8) + 0x20C;
    g_BurstEffectBState1.oriented.brightness = (s8)anim->center.brightness;

    FieldFx_DrawBillboard(output);
    FieldFx_SetSpriteColorMode(0x10);

    i = 0;
    do {
        unsigned int slot = i & 0xFFFF;
        g_BurstEffectBState0.billboard.position.x = anim->position[0][slot] + anim->center.displacement.x;
        g_BurstEffectBState0.billboard.position.y = anim->position[1][slot] + anim->center.displacement.y;
        g_BurstEffectBState0.billboard.position.z = anim->position[2][slot] + anim->center.displacement.z;
        i++;
        g_BurstEffectBState0.billboard.brightness = (s8)anim->center.brightness;
        FieldFx_DrawBillboard(&g_BurstEffectBState0.billboard);
    } while ((i & 0xFFFF) < 8);
}

int BurstEffectB_DrawSparkPoint(void *arg0, void *arg1, FieldFxSparkPoint *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(3);

    lhs_v0 = anim->position.x;
    rhs_v1 = anim->displacement.x;
    output = &g_BurstEffectBState2.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    g_BurstEffectBState2.oriented.position.y = anim->position.y + anim->displacement.y;
    g_BurstEffectBState2.oriented.position.z = anim->position.z + anim->displacement.z;
    g_BurstEffectBState2.oriented.brightness = (s8)anim->brightness;
    FieldFx_DrawBillboard(output);
}

int BurstEffectB_UpdateEnd(void *arg0, FieldFxSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int BurstEffectB_UpdateSparkCloud(void *arg0, FieldFxSlot *state, FieldFxSparkCloud *anim) {
    unsigned int i = 0;

    anim->center.brightness -= 0x10;
    anim->center.scaleStep += 0xC;

    do {
        unsigned int slot = i & 0xFFFF;

        i++;
        anim->position[0][slot] = anim->position[0][slot] + anim->velocity[0][slot];
        anim->position[1][slot] = anim->position[1][slot] + anim->velocity[1][slot];
        anim->position[2][slot] = anim->position[2][slot] + anim->velocity[2][slot];
    } while ((i & 0xFFFF) < 8);

    if ((signed char)anim->center.brightness < 0x10) {
        state->flag = 2;
    }
}

int BurstEffectB_UpdateSparkPoint(void *arg0, FieldFxSlot *state, FieldFxSparkPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldFxSparkPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 4;
    brightness -= 3;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 3;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (signed char)brightness) {
        state->flag = 2;
    }
}

int BurstEffectC_PlantScript(char *object) {
    void **slot = FieldFx_GetSlot(object);

    *slot = &g_BurstEffectCScript;
    g_BurstEffectCState.oriented.scale.x = 0x300;
    g_BurstEffectCState.oriented.scale.y = 0x300;
    g_BurstEffectCState.oriented.scale.z = 0x300;
    g_BurstEffectCState.oriented.clut = 5;
    g_BurstEffectCState.oriented.depth = -100;
    g_BurstEffectCState.oriented.rotation.x = 0;
    g_BurstEffectCState.oriented.rotation.y = 0;
    g_BurstEffectCState.oriented.rotation.z = 0;
    g_BurstEffectCState.oriented.rgb[0] = 0x80;
    g_BurstEffectCState.oriented.rgb[1] = 0x80;
    g_BurstEffectCState.oriented.rgb[2] = 0x80;
    return 0;
}

int BurstEffectC_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffectC_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectCDrawList);
    return 0;
}

int BurstEffectC_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectCUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectCInitList, g_BurstEffectCSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffectC_CloseTarget(obj);
    }
    return 0;
}

int BurstEffectC_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int BurstEffectC_ClassNop6(void) {
    return 0;
}

int BurstEffectC_ClassNop0(void) {
    return 0;
}

int BurstEffectC_InitTarget(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectCTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

    data = g_BurstEffectCTarget;
    g_BurstEffectCTargetPosition.x = data->render_object.target_x;
    g_BurstEffectCTargetPosition.y = data->render_object.target_y;
    g_BurstEffectCTargetPosition.z = data->render_object.target_z;

    FieldFx_EnsureEffectTexturePage(0);
}

int BurstEffectC_InitTimedSpark(void *arg0, void *arg1, FieldFxTimedSpark *anim) {
    int randomX;
    int randomY;
    u16 base;
    int randomZ;

    randomX = rand();
    base = g_BurstEffectCTargetPosition.x - 0x28;
    anim->position.x = base + (randomX % 80);

    randomY = rand();
    base = g_BurstEffectCTargetPosition.y - 0x28;
    anim->position.y = base + (randomY % 80);

    randomZ = rand();
    base = g_BurstEffectCTargetPosition.z;

    anim->brightness = 0x7F;
    anim->reserved04 = 0;
    anim->age = 0;
    base -= 0x28;
    anim->position.z = base + (randomZ % 80);
}

void BurstEffectC_DrawNop(void) {
}

int BurstEffectC_DrawTimedSpark(void *arg0, void *arg1, FieldFxTimedSpark *anim) {
    u16 x;
    FieldBillboard *output;
    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    x = anim->position.x;
    output = &g_BurstEffectCState.billboard;
    output->position.x = x;
    g_BurstEffectCState.billboard.position.y = anim->position.y;
    g_BurstEffectCState.billboard.position.z = anim->position.z;
    g_BurstEffectCState.billboard.brightness = (s8)anim->brightness;
    g_BurstEffectCState.billboard.cell = (anim->age * 2) - 0x60;

    FieldFx_DrawBillboard(output);
    FieldFx_SetSpriteColorMode(0x10);
}

int BurstEffectC_UpdateEnd(void *arg0, char *arg1) {
    int value;

    value = 2;
    arg1[1] = value;
    return value;
}

void BurstEffectC_UpdateTimedSpark(void *arg0, char *arg1, FieldFxTimedSpark *anim) {
    anim->age++;
    if ((signed char)anim->age == 6) {
        arg1[1] = 2;
    }
}
