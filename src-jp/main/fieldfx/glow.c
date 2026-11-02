#include "common.h"
#include "pe1/battle.h"
#include "pe1/field_effect_modules.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/fieldfx_particle.h"
#include "pe1/fieldfx_slot.h"
#include "pe1/fieldfx_state.h"
#include "pe1/fieldfx_task.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_libc.h"

int GlowEffect_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_GlowEffectScript;

    g_GlowEffectSprite0.cell = 0xBD;
    g_GlowEffectSprite0.clut = 9;
    g_GlowEffectSprite0.offset = 0;
    g_GlowEffectSprite0.depth = 0x80;
    g_GlowEffectSprite0.r = 0x80;
    g_GlowEffectSprite0.g = 0x80;
    g_GlowEffectSprite0.b = 0x80;
    g_GlowEffectSprite0.flip = 0;

    g_GlowEffectSprite2.cell = 0xAE;
    g_GlowEffectSprite2.clut = 7;
    g_GlowEffectSprite2.offset = -0x32;
    g_GlowEffectSprite2.depth = 0x80;
    g_GlowEffectSprite2.r = 0x50;
    g_GlowEffectSprite2.g = 0x50;
    g_GlowEffectSprite2.b = 0x50;
    g_GlowEffectSprite2.flip = 0;

    g_GlowEffectSprite1.cell = 0x68;
    g_GlowEffectSprite1.clut = 0;
    g_GlowEffectSprite1.offset = -0x3C;
    g_GlowEffectSprite1.depth = 0x80;
    g_GlowEffectSprite1.r = 0x50;
    g_GlowEffectSprite1.g = 0x50;
    g_GlowEffectSprite1.b = 0x50;
    g_GlowEffectSprite1.flip = 0;

    g_GlowEffectSprite3.cell = 0x42;
    g_GlowEffectSprite3.clut = 0x20;
    g_GlowEffectSprite3.offset = 0x32;
    g_GlowEffectSprite3.depth = 0x80;
    g_GlowEffectSprite3.r = 0x80;
    g_GlowEffectSprite3.g = 0x80;
    g_GlowEffectSprite3.b = 0x80;
    g_GlowEffectSprite3.flip = 0;

    return 0;
}

int GlowEffect_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffect_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectDrawList);
    return 0;
}
int GlowEffect_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectInitList, g_GlowEffectSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffect_CloseTarget(obj);
    }
    return 0;
}
int GlowEffect_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffect_ClassNop6(void) {
    return 0;
}
int GlowEffect_ClassNop0(void) {
    return 0;
}

int GlowEffect_InitOwner(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    g_GlowEffectPosition.x = data->render_object.matrices[19].translation[0];
    g_GlowEffectPosition.y = data->render_object.matrices[19].translation[1];
    g_GlowEffectPosition.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectOwner = data;
    FieldFx_EnsureEffectTexturePage(0);
}

int GlowEffect_InitParticle(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = g_GlowEffectOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = g_GlowEffectPosition.x;
    anim->position.y = g_GlowEffectPosition.y;
    anim->position.z = g_GlowEffectPosition.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

int GlowEffect_InitFlareLayers(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    GteShortVector *entry;
    FieldActor *data;
    RenderMatrix *model;

    entry = &g_GlowEffectFlareOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_GlowEffectPosition.x + out.x;
    anim_s0->position.y = g_GlowEffectPosition.y + out.y;
    data = g_GlowEffectOwner;
    anim_s0->position.z = g_GlowEffectPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int GlowEffect_InitCoreGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;
    RenderMatrix *model;

    in = g_GlowEffectPointOffset;

    model = g_GlowEffectOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)&model[19], &in, &out);

    anim->position.x = g_GlowEffectPosition.x + out.x;
    anim->position.y = g_GlowEffectPosition.y + out.y;
    anim->position.z = g_GlowEffectPosition.z + out.z;
    anim->brightness = 0x7F;
    anim->scale = 0;
}

int GlowEffect_InitGroundGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    BattleEntity *entity_v1 = D_8009CD10;
    int value;
    int z_v1;

    anim->position.x = g_GlowEffectPosition.x;
    value = entity_v1->posY.parts.integer;
    anim->position.y = value;
    z_v1 = g_GlowEffectPosition.z;
    anim->brightness = 0x7F;
    anim->scale = 0x224;
    anim->position.z = z_v1;
}

int GlowEffect_InitHaloGlow(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    RenderMatrix *model;
    GteShortVector *entry;

    entry = &g_GlowEffectHaloOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_GlowEffectPosition.x + out.x;
    anim_s0->position.y = g_GlowEffectPosition.y + out.y;
    data = g_GlowEffectOwner;
    anim_s0->position.z = g_GlowEffectPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void GlowEffect_DrawNop(void) {
}

int GlowEffect_DrawParticle(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = (u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim->age << 6;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x10, 0x10);
    FieldFx_SetSpriteBlendMode(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = g_GlowEffectScaleByAction[index];
    scale.y = g_GlowEffectScaleByAction[index];
    scale.z = g_GlowEffectScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_GlowEffectSprite0, &matrix, 1);
}

#define GLOW_LAYERS_FUNC GlowEffect_DrawFlareLayers
#define GLOW_LAYERS_SPIN_A g_GlowEffectLayersSpinA
#define GLOW_LAYERS_SPIN_B g_GlowLayers1SpinB
#define GLOW_LAYERS_SCALE g_GlowLayers1Scale
#define GLOW_LAYERS_SPRITE g_GlowEffectSprite2
#include "glow_layers.inc"

int GlowEffect_DrawPointGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    u16 *field_s3 = &g_GlowEffectSprite1.depth;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    scale = anim->scale + 0x170;
    field = anim->brightness;

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    *field_s3 = field;
    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    FieldFx_DrawGlowSprite(&g_GlowEffectSprite1, &matrix, 1);
}

int GlowEffect_DrawGroundGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 *field = &g_GlowEffectSprite3.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_GlowEffectSpriteRotation;

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
    FieldFx_DrawGlowSprite(&g_GlowEffectSprite3, &matrix, 0);
}

int GlowEffect_DrawHaloGlow(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    u16 *field = &g_GlowEffectSprite3.depth;
    GteMatrix matrix;
    GteVector scaleCopy;
    GteVector scale;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    *field = anim->brightness;
    matrix = anim->transform.matrix;

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_GlowEffectSprite3, &matrix, 0);
}

int GlowEffect_UpdateEnd(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int GlowEffect_UpdateParticle(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
    u16 vy = anim->velocity.y;
    u16 vz = anim->velocity.z;
    int age;
    int lifetime;

    anim->position.x += anim->velocity.x;
    anim->position.y += vy;
    anim->position.z += vz;
    anim->velocity.y += 3;
    age = anim->age;
    anim->age = age + 1;
    if ((s16)anim->position.y > 0) {
        anim->velocity.y = -anim->velocity.y;
    }
    lifetime = particle->lifetime;
    particle->lifetime = lifetime - 1;
    if (lifetime == 0) {
        state->flag = 2;
    }
}
void GlowEffect_UpdateFlareLayers(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 0x14;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void GlowEffect_UpdateCoreGlow(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x3C;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void GlowEffect_UpdateGroundGlow(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x78;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void GlowEffect_UpdateHaloGlow(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0xA;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}

int GlowEffectB_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_GlowEffectBScript;

    g_GlowEffectBSprite0.cell = 0xBD;
    g_GlowEffectBSprite0.clut = 9;
    g_GlowEffectBSprite0.offset = 0;
    g_GlowEffectBSprite0.depth = 0x80;
    g_GlowEffectBSprite0.r = 0x80;
    g_GlowEffectBSprite0.g = 0x80;
    g_GlowEffectBSprite0.b = 0x80;
    g_GlowEffectBSprite0.flip = 0;

    g_GlowEffectBSprite2.cell = 0xAE;
    g_GlowEffectBSprite2.clut = 7;
    g_GlowEffectBSprite2.offset = -0x32;
    g_GlowEffectBSprite2.depth = 0x80;
    g_GlowEffectBSprite2.r = 0x50;
    g_GlowEffectBSprite2.g = 0x50;
    g_GlowEffectBSprite2.b = 0x50;
    g_GlowEffectBSprite2.flip = 0;

    g_GlowEffectBSprite1.cell = 0x68;
    g_GlowEffectBSprite1.clut = 0;
    g_GlowEffectBSprite1.offset = 0;
    g_GlowEffectBSprite1.depth = 0x80;
    g_GlowEffectBSprite1.r = 0x50;
    g_GlowEffectBSprite1.g = 0x50;
    g_GlowEffectBSprite1.b = 0x50;
    g_GlowEffectBSprite1.flip = 0;

    return 0;
}

int GlowEffectB_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffectB_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectBDrawList);
    return 0;
}
int GlowEffectB_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectBUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectBInitList, g_GlowEffectBSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffectB_CloseTarget(obj);
    }
    return 0;
}
int GlowEffectB_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffectB_ClassNop6(void) {
    return 0;
}
int GlowEffectB_ClassNop0(void) {
    return 0;
}

int GlowEffectB_InitOwner(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    g_GlowEffectBPosition.x = data->render_object.matrices[19].translation[0];
    g_GlowEffectBPosition.y = data->render_object.matrices[19].translation[1];
    g_GlowEffectBPosition.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectBOwner = data;
    FieldFx_EnsureEffectTexturePage(0);
}

int GlowEffectB_InitParticle(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)g_GlowEffectBOwner->render_object.matrices, &vec, &anim->velocity);

    {
        u16 z;

        anim->position.x = g_GlowEffectBPosition.x;
        anim->position.y = g_GlowEffectBPosition.y;
        z = g_GlowEffectBPosition.z;
        anim->lifetime = 0x14;
        anim->age = 0;
        anim->position.z = z;
    }
}

int GlowEffectB_InitFlareLayers(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_GlowEffectBFlareOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectBOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_GlowEffectBPosition.x + out.x;
    anim_s0->position.y = g_GlowEffectBPosition.y + out.y;
    data = g_GlowEffectBOwner;
    anim_s0->position.z = g_GlowEffectBPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int GlowEffectB_InitPointGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;

    in = g_GlowEffectBPointOffset;
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectBOwner->render_object.matrices[19], &in, &out);

    {
        u16 y;
        u16 z;

        anim->position.x = g_GlowEffectBPosition.x + out.x;
        y = g_GlowEffectBPosition.y + out.y;
        anim->position.y = y;
        z = g_GlowEffectBPosition.z + out.z;
        anim->brightness = 0x7F;
        anim->scale = 0;
        anim->position.z = z;
    }
}

void GlowEffectB_DrawNop(void) {
}

int GlowEffectB_DrawParticle(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = (u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim->age << 6;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x10, 0x10);
    FieldFx_SetSpriteBlendMode(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = g_GlowEffectBScaleByAction[index];
    scale.y = g_GlowEffectBScaleByAction[index];
    scale.z = g_GlowEffectBScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_GlowEffectBSprite0, &matrix, 1);
}

#define GLOW_LAYERS_FUNC GlowEffectB_DrawFlareLayers
#define GLOW_LAYERS_SPIN_A g_GlowEffectBLayersSpinA
#define GLOW_LAYERS_SPIN_B g_GlowEffectBSpinB
#define GLOW_LAYERS_SCALE g_GlowEffectBLayersScale
#define GLOW_LAYERS_SPRITE g_GlowEffectBSprite2
#include "glow_layers.inc"

int GlowEffectB_DrawPointGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 *field_s3 = &g_GlowEffectBSprite1.depth;
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    scale = anim->scale + 0x170;
    field = anim->brightness;

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    *field_s3 = field;
    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    FieldFx_DrawGlowSprite(&g_GlowEffectBSprite1, &matrix, 1);
}

int GlowEffectB_UpdateEnd(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int GlowEffectB_UpdateParticle(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
    u16 vy = anim->velocity.y;
    u16 vz = anim->velocity.z;
    int age;
    int lifetime;

    anim->position.x += anim->velocity.x;
    anim->position.y += vy;
    anim->position.z += vz;
    anim->velocity.y += 3;
    age = anim->age;
    anim->age = age + 1;
    if ((s16)anim->position.y > 0) {
        anim->velocity.y = -anim->velocity.y;
    }
    lifetime = particle->lifetime;
    particle->lifetime = lifetime - 1;
    if (lifetime == 0) {
        state->flag = 2;
    }
}
int GlowEffectB_UpdateFlareLayers(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
int GlowEffectB_UpdatePointGlow(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x28;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int GlowEffectC_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_GlowEffectCScript;

    g_GlowEffectCSprite0.cell = 0xBD;
    g_GlowEffectCSprite0.clut = 9;
    g_GlowEffectCSprite0.offset = 0;
    g_GlowEffectCSprite0.depth = 0x80;
    g_GlowEffectCSprite0.r = 0x80;
    g_GlowEffectCSprite0.g = 0x80;
    g_GlowEffectCSprite0.b = 0x80;
    g_GlowEffectCSprite0.flip = 0;

    g_GlowEffectCSprite1.cell = 0xAC;
    g_GlowEffectCSprite1.clut = 6;
    g_GlowEffectCSprite1.offset = -0x32;
    g_GlowEffectCSprite1.depth = 0x80;
    g_GlowEffectCSprite1.r = 0x50;
    g_GlowEffectCSprite1.g = 0x50;
    g_GlowEffectCSprite1.b = 0x50;
    g_GlowEffectCSprite1.flip = 0;

    return 0;
}
int GlowEffectC_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffectC_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectCDrawList);
    return 0;
}
int GlowEffectC_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectCUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectCInitList, g_GlowEffectCSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffectC_CloseTarget(obj);
    }
    return 0;
}
int GlowEffectC_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffectC_ClassNop6(void) {
    return 0;
}
int GlowEffectC_ClassNop0(void) {
    return 0;
}

int GlowEffectC_InitOwner(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    g_GlowEffectCPosition.x = data->render_object.matrices[19].translation[0];
    g_GlowEffectCPosition.y = data->render_object.matrices[19].translation[1];
    g_GlowEffectCPosition.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectCOwner = data;
    FieldFx_EnsureEffectTexturePage(0);
}

int GlowEffectC_InitParticle(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = g_GlowEffectCOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = g_GlowEffectCPosition.x;
    anim->position.y = g_GlowEffectCPosition.y;
    anim->position.z = g_GlowEffectCPosition.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

int GlowEffectC_InitFlareLayers(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_GlowEffectCFlareOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectCOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_GlowEffectCPosition.x + out.x;
    anim_s0->position.y = g_GlowEffectCPosition.y + out.y;
    data = g_GlowEffectCOwner;
    anim_s0->position.z = g_GlowEffectCPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

void GlowEffectC_DrawNop(void) {
}

int GlowEffectC_DrawParticle(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = (u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim->age << 6;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x10, 0x10);
    FieldFx_SetSpriteBlendMode(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = g_GlowEffectCScaleByAction[index];
    scale.y = g_GlowEffectCScaleByAction[index];
    scale.z = g_GlowEffectCScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_GlowEffectCSprite0, &matrix, 1);
}

#define GLOW_LAYERS_FUNC GlowEffectC_DrawFlareLayers
#define GLOW_LAYERS_SPIN_A g_GlowEffectCLayersSpinA
#define GLOW_LAYERS_SPIN_B g_GlowEffectCLayersSpinB
#define GLOW_LAYERS_SCALE g_GlowLayers3Scale
#define GLOW_LAYERS_SPRITE g_GlowEffectCSprite1
#include "glow_layers.inc"

int GlowEffectC_UpdateEnd(void *arg0, FieldFxSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int GlowEffectC_UpdateParticle(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
    u16 vy = anim->velocity.y;
    u16 vz = anim->velocity.z;
    int age;
    int lifetime;

    anim->position.x += anim->velocity.x;
    anim->position.y += vy;
    anim->position.z += vz;
    anim->velocity.y += 3;
    age = anim->age;
    anim->age = age + 1;
    if ((s16)anim->position.y > 0) {
        anim->velocity.y = -anim->velocity.y;
    }
    lifetime = particle->lifetime;
    particle->lifetime = lifetime - 1;
    if (lifetime == 0) {
        state->flag = 2;
    }
}

int GlowEffectC_UpdateFlareLayers(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int FourLayerGlow_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_FourLayerGlowScript;

    g_FourLayerGlowSprite0.cell = 0xBD;
    g_FourLayerGlowSprite0.clut = 9;
    g_FourLayerGlowSprite0.offset = 0;
    g_FourLayerGlowSprite0.depth = 0x80;
    g_FourLayerGlowSprite0.r = 0x80;
    g_FourLayerGlowSprite0.g = 0x80;
    g_FourLayerGlowSprite0.b = 0x80;
    g_FourLayerGlowSprite0.flip = 0;

    g_FourLayerGlowSprite2.cell = 0xAE;
    g_FourLayerGlowSprite2.clut = 7;
    g_FourLayerGlowSprite2.offset = -0x32;
    g_FourLayerGlowSprite2.depth = 0x80;
    g_FourLayerGlowSprite2.r = 0x50;
    g_FourLayerGlowSprite2.g = 0x50;
    g_FourLayerGlowSprite2.b = 0x50;
    g_FourLayerGlowSprite2.flip = 0;

    g_FourLayerGlowSprite1.cell = 0x68;
    g_FourLayerGlowSprite1.clut = 0;
    g_FourLayerGlowSprite1.offset = -0x3C;
    g_FourLayerGlowSprite1.depth = 0x80;
    g_FourLayerGlowSprite1.r = 0x50;
    g_FourLayerGlowSprite1.g = 0x50;
    g_FourLayerGlowSprite1.b = 0x50;
    g_FourLayerGlowSprite1.flip = 0;

    g_FourLayerGlowSprite3.cell = 0x42;
    g_FourLayerGlowSprite3.clut = 0x20;
    g_FourLayerGlowSprite3.offset = 0x32;
    g_FourLayerGlowSprite3.depth = 0x80;
    g_FourLayerGlowSprite3.r = 0x80;
    g_FourLayerGlowSprite3.g = 0x80;
    g_FourLayerGlowSprite3.b = 0x80;
    g_FourLayerGlowSprite3.flip = 0;

    return 0;
}

int FourLayerGlow_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int FourLayerGlow_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_FourLayerGlowDrawList);
    return 0;
}

int FourLayerGlow_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_FourLayerGlowUpdateList);
    second = FieldFx_RunScript(obj, g_FourLayerGlowInitList, g_FourLayerGlowSlotSizes);
    status = first | second;
    if (status == -1) {
        FourLayerGlow_CloseTarget(obj);
    }
    return 0;
}

int FourLayerGlow_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int FourLayerGlow_ClassNop6(void) {
    return 0;
}

int FourLayerGlow_ClassNop0(void) {
    return 0;
}

int FourLayerGlow_InitOwner(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    g_FourLayerGlowPosition.x = data->render_object.matrices[19].translation[0];
    g_FourLayerGlowPosition.y = data->render_object.matrices[19].translation[1];
    g_FourLayerGlowPosition.z = data->render_object.matrices[19].translation[2];
    g_FourLayerGlowOwner = data;
    FieldFx_EnsureEffectTexturePage(0);
}

int FourLayerGlow_InitParticle(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)g_FourLayerGlowOwner->render_object.matrices, &vec, &anim->velocity);

    {
        u16 z;

        anim->position.x = g_FourLayerGlowPosition.x;
        anim->position.y = g_FourLayerGlowPosition.y;
        z = g_FourLayerGlowPosition.z;
        anim->lifetime = 0x14;
        anim->age = 0;
        anim->position.z = z;
    }
}

int FourLayerGlow_InitFlareLayers(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_FourLayerGlowFlareOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_FourLayerGlowPosition.x + out.x;
    anim_s0->position.y = g_FourLayerGlowPosition.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position.z = g_FourLayerGlowPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int FourLayerGlow_InitTwinGlow(void *arg0, void *arg1, FieldFxTwinGlow *anim) {
    FieldFxTwinGlow *anim_s0;
    u16 *field_s1;
    GteShortVector *out_s2;
    u16 z_base_v1;
    u16 z_out_a0;
    GteShortVector in0;
    GteShortVector in1;
    GteShortVector out;
    FieldActor *data;

    anim_s0 = anim;

    in0 = g_FourLayerGlowPointOffsetA;
    in1 = g_FourLayerGlowPointOffsetB;
    out_s2 = &out;

    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], &in0, out_s2);
    field_s1 = &g_FourLayerGlowPosition.x;
    anim_s0->position[0].x = field_s1[0] + out.x;
    anim_s0->position[0].y = g_FourLayerGlowPosition.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position[0].z = g_FourLayerGlowPosition.z + out.z;

    ApplyMatrixSV((GteMatrix *)&data->render_object.matrices[19], &in1, out_s2);
    anim_s0->position[1].x = field_s1[0] + out.x;
    anim_s0->position[1].y = g_FourLayerGlowPosition.y + (u16)out.y;
    z_base_v1 = g_FourLayerGlowPosition.z;
    z_out_a0 = out.z;
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0;
    anim_s0->position[1].z = z_base_v1 + z_out_a0;
}

int FourLayerGlow_InitGroundGlow(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 x = g_FourLayerGlowPosition.x;
    BattleEntity *data = D_8009CD10;
    int y;
    u16 z;

    anim->position.x = x;
    y = data->posY.parts.integer;
    anim->position.y = y;
    z = g_FourLayerGlowPosition.z;
    anim->brightness = 0x7F;
    anim->scale = 0x224;
    anim->position.z = z;
}

int FourLayerGlow_InitHaloGlow(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_FourLayerGlowHaloOffsets[(short)((u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = g_FourLayerGlowPosition.x + out.x;
    anim_s0->position.y = g_FourLayerGlowPosition.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position.z = g_FourLayerGlowPosition.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void FourLayerGlow_DrawNop(void) {
}

int FourLayerGlow_DrawParticle(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = (u16)((Combatant *)D_8009CD10->core)->action->actionCode.actionId - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim->age << 6;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x10, 0x10);
    FieldFx_SetSpriteBlendMode(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = g_FourLayerGlowScaleByAction[index];
    scale.y = g_FourLayerGlowScaleByAction[index];
    scale.z = g_FourLayerGlowScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite0, &matrix, 1);
}

/* Four-layer variant of the field glow: two pairs of fixed spins, the
 * second pair pushed further out and drawn at the full depth.
 * Matching debt: 6 register pins and the remaining empty constraints.
 * Explicit spin address aliases retain the retail spill order. Rotation,
 * column, and translation windows use the stock GTE macros. */
void FourLayerGlow_DrawFlareLayers(void *object, void *slot, FieldGlowLayers *glow)
{
    int halfBrightness;
    u16 brightness;
    GteMatrix *workMatrix;
    register GteMatrix *stepMatrix asm("$18");

    GteShortVector *spinCAddress;
    GteShortVector *spinDAddress;

    register const volatile u16 *column1 asm("$19");
    register const volatile u16 *column2 asm("$22");
    register u16 *outColumn1 asm("$20");
    u16 *outColumn2;
    const s32 *translation;
    s32 *outTranslation;
    register GteShortVector *nextSpin asm("$23");
    GteShortVector *thirdSpin;
    GteShortVector *fourthSpin;
    GteMatrix matrix;
    GteMatrix rotation;
    GteShortVector spinA;
    GteShortVector spinB;
    GteShortVector spinC;
    GteShortVector spinD;
    GteVector scaleA;
    GteVector scaleB;
    GteVector scaleC;
    GteVector scaleD;

    spinA = g_FourLayerGlowSpinA;
    spinB = g_FourLayerGlowSpinB;
    spinCAddress = &spinC;
    spinDAddress = &spinD;
    spinC = g_FourLayerGlowSpinC;
    spinD = g_FourLayerGlowSpinD;
    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x10);
    FieldFx_SetSpriteBlendMode(2);
    halfBrightness = (s16)glow->depth >> 1;
    g_FourLayerGlowSprite2.offset = -10;
    g_FourLayerGlowSprite2.depth = halfBrightness;

    RotMatrixYXZ(&spinA, &rotation);
    stepMatrix = &rotation;
        matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = -20;
    nextSpin = &spinB;
    thirdSpin = spinCAddress;
    fourthSpin = spinDAddress;
    workMatrix = &matrix;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    column1 = (const volatile u16 *)&rotation + 1;
        gte_ldclmv(column1);
    gte_rtir();
    outColumn1 = (u16 *)&matrix + 1;
        gte_stclmv(outColumn1);
    outColumn2 = (u16 *)&rotation + 2;
    column2 = outColumn2;
        gte_ldclmv(column2);
    gte_rtir();
    outColumn2 = (u16 *)&matrix + 2;
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    translation = rotation.t;
            gte_ldlv0(translation);
    gte_rt();
    outTranslation = matrix.t;
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleA = g_FourLayerGlowScale;
    ScaleMatrix(workMatrix, &scaleA);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite2, workMatrix, 0);

    RotMatrixYXZ(nextSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleB = g_FourLayerGlowScale;
    ScaleMatrix(workMatrix, &scaleB);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite2, workMatrix, 0);

    brightness = glow->depth;
    g_FourLayerGlowSprite2.offset = -50;
    g_FourLayerGlowSprite2.depth = brightness;

    RotMatrixYXZ(thirdSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = -10;
    rotation.t[2] = -400;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleC = g_FourLayerGlowScale;
    ScaleMatrix(workMatrix, &scaleC);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite2, workMatrix, 0);

    RotMatrixYXZ(fourthSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

            scaleD = g_FourLayerGlowScale;
    ScaleMatrix(workMatrix, &scaleD);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite2, workMatrix, 0);
}

int FourLayerGlow_DrawTwinGlow(void *arg0, void *arg1, FieldFxTwinGlow *anim) {
    u16 *field_s4 = &g_FourLayerGlowSprite1.depth;
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    int localScale2[4];

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    scale = anim->scale + 0x42C;
    field = anim->brightness;

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    *field_s4 = field;
    matrix.t[0] = (s16)anim->position[0].x;
    matrix.t[1] = (s16)anim->position[0].y;
    matrix.t[2] = (s16)anim->position[0].z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite1, &matrix, 1);

    matrix.m[2][2] = 0x1000;
    matrix.m[1][1] = 0x1000;
    matrix.m[0][0] = 0x1000;
    matrix.t[2] = 0;
    matrix.t[1] = 0;
    matrix.t[0] = 0;
    matrix.m[2][1] = 0;
    matrix.m[2][0] = 0;
    matrix.m[1][2] = 0;
    matrix.m[1][0] = 0;
    matrix.m[0][2] = 0;
    matrix.m[0][1] = 0;

    matrix.t[0] = (s16)anim->position[1].x;
    matrix.t[1] = (s16)anim->position[1].y;
    matrix.t[2] = (s16)anim->position[1].z;

    memset(localScale2, 0, sizeof(localScale2));
    localScale2[0] = scale;
    localScale2[1] = scale;
    localScale2[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale2);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite1, &matrix, 1);
}

int FourLayerGlow_DrawGroundGlow(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    s16 *position;
    u16 *field = &g_FourLayerGlowSprite3.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_FourLayerGlowSpriteRotation;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    *field = anim->brightness;
    RotMatrix(&rot, &matrix);

    position = &anim->position.x;
    matrix.t[0] = position[0];
    matrix.t[1] = position[1];
    matrix.t[2] = position[2];

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite3, &matrix, 0);
}

int FourLayerGlow_DrawHaloGlow(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    u16 *field = &g_FourLayerGlowSprite3.depth;
    GteMatrix matrix;
    GteVector scaleCopy;
    GteVector scale;

    FieldFx_SetSpriteTexturePreset(3);
    FieldFx_SetSpriteColorMode(0x100);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteBlendMode(2);

    *field = anim->brightness;
    matrix = anim->transform.matrix;

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    FieldFx_DrawGlowSprite(&g_FourLayerGlowSprite3, &matrix, 0);
}

int FourLayerGlow_UpdateEnd(void *arg0, FieldFxSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int FourLayerGlow_UpdateParticle(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
    u16 vy = anim->velocity.y;
    u16 vz = anim->velocity.z;
    int age;
    int lifetime;

    anim->position.x += anim->velocity.x;
    anim->position.y += vy;
    anim->position.z += vz;
    anim->velocity.y += 3;
    age = anim->age;
    anim->age = age + 1;
    if ((s16)anim->position.y > 0) {
        anim->velocity.y = -anim->velocity.y;
    }
    lifetime = particle->lifetime;
    particle->lifetime = lifetime - 1;
    if (lifetime == 0) {
        state->flag = 2;
    }
}

int FourLayerGlow_UpdateFlareLayers(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int FourLayerGlow_UpdateTwinGlow(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int FourLayerGlow_UpdateGroundGlow(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x78;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int FourLayerGlow_UpdateHaloGlow(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0xA;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
