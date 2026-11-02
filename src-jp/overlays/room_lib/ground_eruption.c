/*
 * The ground eruption: its module class and the eruption it runs.
 *
 * The class is the seven methods the class table lists in front of the
 * eruption. The registration and the start method act only while the
 * field engine runs the object (status 3), and the close method clears the
 * target's owner bits without marking the target closed (compare
 * RoomLib_CloseTarget). In room_m146 the class comes before the falling
 * sprite burst's module, which uses the shared class methods. The script
 * and the lists are each overlay's own data (pe1/room_ground_eruption.h).
 *
 * room_m141, m146, m153, m154, m328 and scene_e02, e04 and e05 link the
 * class and the eruption directly after each other, with the eruption's
 * seeds and scales as the unit's only rodata, so they are one unit.
 */

#include "pe1/pe_image.h"
#include "pe1/random.h"
#include "pe1/room_ground_eruption.h"

#include "render_layouts.h"
#include "room_lib.h"

ROOMLIB_RETURN_ZERO(RoomEffect_GroundEruptionNop0)

ROOMLIB_PLANT_TABLE(RoomEffect_GroundEruptionPlantScript, g_RoomEruptionScript)

ROOMLIB_SPAWN6(RoomEffect_GroundEruptionSpawn6)

ROOMLIB_REGISTER_TABLE_AT3(RoomEffect_GroundEruptionRegister, g_RoomEruptionDrawList)

ROOMLIB_START_AT3(RoomEffect_GroundEruptionStart, g_RoomEruptionUpdateList,
                  g_RoomEruptionInitList, g_RoomEruptionSpawnLayout,
                  RoomEffect_GroundEruptionRelease)

int RoomEffect_GroundEruptionRelease(RoomEnt *o) {
    o->state = 4;
    if ((unsigned int)FieldFx_GetStatus(o) >= 2) {
        RoomRenderNode *target = o->link->target;
        target->flags &= 0xC0FFFFFF;
    }
    return 0;
}

ROOMLIB_RETURN_ZERO(RoomEffect_GroundEruptionNop6)

/*
 * The ground eruption: a column that rises from the floor under its owner,
 * with a flash at the owner's feet, a sixteen-point shock ring that spreads
 * along the floor, a floor shadow, and six debris particles flung out and
 * falling back with trails. While it runs, a hit on the player sets the
 * player's 0x4000 status flag.
 *
 * room_m141, m146, m153, m154, m328 and scene_e02, e04 and e05 link these
 * eighteen functions in this order right after the module's class methods,
 * with the same 0x50 bytes of seeds and scales after the 0xC-byte room
 * header; this unit is that object, compiled into each of them. The four
 * sprite packets are room data (pe1/room_ground_eruption.h); in scene_e04
 * and scene_e05 they lie past the end of the extracted image and are named
 * at their addresses.
 */

static const RoomFxSeed8 s_EruptionColumnSeed = {
    { 0x00, 0x04, 0, 0, 0, 0, 0, 0 }
};
static const RoomFxVec4 s_EruptionGlowScale = { 0x400, 0x400, 0x1000, 0 };
static const RoomFxVec4 s_EruptionRingScale = { 0x6D4, 0x6D4, 0x6D4, 0 };
static const RoomFxVec4 s_EruptionRingPointScale = { 0x338, 0x338, 0x1000, 0 };
static const RoomFxSeed8 s_EruptionShadowSeed = {
    { 0, 0, 0, 0, 0x00, 0x04, 0, 0 }
};
static const RoomFxVec4 s_EruptionDebrisScale = { 0x226, 0x226, 0x226, 0 };

/* Sets up the four packets and the column's work area, then hands the work
 * area to the field engine's effect handler 0x587. */
void RoomEffect_GroundEruptionInit(RoomEnt *owner, void *arg1, RoomEruptionActor *state) {
    RoomLink *entity;
    int asset;
    register RoomEnt *callOwner asm("$4");
    int depth;
    u8 color;
    s16 offset;
    int modeB;
    RoomEruptionActor *workState;
    RoomSpriteMatrix *workMatrix;

    asm("" : "=r"(workState) : "0"(state));
    FieldFx_SetScriptContext(workState);
    workState->cooldown = 0;
    workState->collisionState = 0;
    workState->reload = *FieldFx_VarPtr(5);
    workState->collisionRadius = *FieldFx_VarPtr(4);
    entity = owner->link;
    workState->entity = entity;
    workState->matrix = *(RoomSpriteMatrix *)entity->p238;
    workState->matrix.t[0] = *FieldFx_VarPtr(1);
    workState->matrix.t[1] = *FieldFx_VarPtr(2);
    workState->matrix.t[2] = *FieldFx_VarPtr(3);
    asset = Asset_SearchByKeyType(0xA6);
    callOwner = owner;
    workState->asset.handle = asset;

    g_RoomEruptionGlowPacket.code = 0x42;
    g_RoomEruptionGlowPacket.mode = 3;
    {
        int minus50;
        minus50 = -50;
        depth = 0x80;
        asm volatile("" : "=r"(color) : "0"((u8)0x80));
        g_RoomEruptionGlowPacket.offset = minus50;
    }
    g_RoomEruptionRingPacket.code = 0x20;
    g_RoomEruptionRingPacket.mode = 1;
    g_RoomEruptionRingPacket.offset = -51;
    g_RoomEruptionShadowPacket.code = 0x40;
    {
        int two;
        two = 2;
        asm volatile("" : "=r"(offset) : "0"((s16)-41));
        g_RoomEruptionShadowPacket.mode = two;
    }
    g_RoomEruptionDebrisPacket.code = 0x47;
    modeB = 5;
    PE1_COMPILER_LAUNDER(workState);
    workMatrix = &workState->matrix;
    PE1_COMPILER_LAUNDER(workMatrix);
    g_RoomEruptionGlowPacket.depth = depth;
    g_RoomEruptionRingPacket.depth = depth;
    g_RoomEruptionShadowPacket.depth = depth;
    g_RoomEruptionDebrisPacket.depth = depth;
    g_RoomEruptionShadowPacket.offset = offset;
    g_RoomEruptionDebrisPacket.offset = offset;
    g_RoomEruptionGlowPacket.r = color;
    g_RoomEruptionGlowPacket.g = color;
    g_RoomEruptionGlowPacket.b = color;
    g_RoomEruptionGlowPacket.zero = 0;
    g_RoomEruptionRingPacket.r = color;
    g_RoomEruptionRingPacket.g = color;
    g_RoomEruptionRingPacket.b = color;
    g_RoomEruptionRingPacket.zero = 0;
    g_RoomEruptionShadowPacket.r = color;
    g_RoomEruptionShadowPacket.g = color;
    g_RoomEruptionShadowPacket.b = color;
    g_RoomEruptionShadowPacket.zero = 0;
    g_RoomEruptionDebrisPacket.mode = modeB;
    g_RoomEruptionDebrisPacket.r = color;
    g_RoomEruptionDebrisPacket.g = color;
    g_RoomEruptionDebrisPacket.b = color;
    g_RoomEruptionDebrisPacket.zero = 0;
    g_RoomFloorY->y = 0;
    FieldFx_PlaySoundAtMatrixPosition(callOwner, 0x587, workMatrix);
}

void RoomEffect_GroundEruptionNop(void) {
}

/* Fires once the owner's target reaches state 2 while armed: sets the
 * player's 0x4000 flag and marks the target, then waits the reload time. */
void RoomEffect_GroundEruptionTick(RoomEnt *ent, unsigned char *signal,
                                   RoomTimer2 *timer) {
    RoomTimer2 *work;
    int **flagsBase;
    int *flags;
    RoomRenderNode *target;
    unsigned short reload;

    work = timer;
    if (timer->h2A == 0) {
        if (work->h2C == 1) {
            if (FieldFx_GetStatus(ent) == 3) {
                if (ent->link->target->state[0] == 2) {
                    reload = work->h28;
                    flagsBase = (int **)D_8009CD10;
                    work->h2C = 0;
                    timer->h2A = reload;

                    flags = *flagsBase;
                    flags[0x13] |= 0x4000;

                    target = ent->link->target;
                    target->flags |= 0x80000000;
                }
            }
        }
    } else {
        work->h2A = timer->h2A - 1;
        work->h2C = 0;
    }

    if (FieldFx_IsLastSlotHalted() == 1) {
        signal[1] = 2;
    }
}

/* Emitter parameters of the rising column. */
void RoomEffect_GroundEruptionInitColumn(void *ent, void *unused,
                                         RoomEruptionColumn *state) {
    state->scale.signedValue = 0x5DC;
    state->depth = 0xFF;
    state->emitter.mode = 0x10;
    state->emitter.color0[0] = 0x40;
    state->emitter.color0[1] = 0x20;
    state->emitter.color0[2] = 0x10;
    state->emitter.color1[0] = 0x80;
    state->emitter.color1[1] = 0x80;
    state->emitter.color1[2] = 0x80;
    state->emitter.extent0 = 0xF0;
    state->emitter.extent1 = 0xA;
    state->speed.signedValue = 0;
    state->emitter.offset = 0;
    state->emitter.source = state->sourceData;
    FieldFx_BuildRingGeometry(&state->emitter);
}

/* The retail function is marked handwritten and uses four explicit GTE windows. */
void RoomEffect_GroundEruptionTransformColumn(void *arg0, void *arg1,
                                              RoomEruptionColumn *state) {
    RoomLibSpriteTransformStack stack;
    RoomSpriteMatrix *matrix;
    RoomSpriteMatrix *scaleMatrix;
    RoomEruptionColumn *workState;
    RoomEruptionActor *owner;

    workState = state;
    owner = FieldFx_GetScriptContext();
    stack.seed = s_EruptionColumnSeed;

    stack.scaleMatrix.m[2][2] = 0x1000;
    stack.scaleMatrix.m[1][1] = 0x1000;
    stack.scaleMatrix.m[0][0] = 0x1000;
    stack.scaleMatrix.t[2] = 0;
    stack.scaleMatrix.t[1] = 0;
    stack.scaleMatrix.t[0] = 0;
    stack.scaleMatrix.m[2][1] = 0;
    stack.scaleMatrix.m[2][0] = 0;
    stack.scaleMatrix.m[1][2] = 0;
    stack.scaleMatrix.m[1][0] = 0;
    stack.scaleMatrix.m[0][2] = 0;
    stack.scaleMatrix.m[0][1] = 0;
    memset(&stack.sourceScale, 0, 0x10);

    stack.sourceScale.x = workState->scale.signedValue;
    stack.sourceScale.y = workState->scale.signedValue;
    stack.sourceScale.z = workState->scale.signedValue;
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    ScaleMatrix(scaleMatrix, &stack.scale);
    RotMatrix(&stack.seed.vector, &stack.matrix);
    matrix = &stack.matrix;

    gte_ldrotmatrix(scaleMatrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][0]);
    gte_stir123_column(matrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][1]);
    gte_stir123_column_at(&stack.matrix.m[0][1]);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][2]);
    gte_stir123_column_at(&stack.matrix.m[0][2]);
    gte_ldtransmatrix(scaleMatrix);
    {
        s32 *translation = stack.matrix.t;
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(translation);
        gte_swc2_10_4(translation);
        gte_swc2_11_8(translation);
    }

    FieldFx_SetSpriteBlendMode(2);
    workState->emitter.intensity = (u16)workState->depth;
    stack.matrix.t[0] = owner->matrix.t[0];
    stack.matrix.t[1] = owner->matrix.t[1];
    stack.matrix.t[2] = owner->matrix.t[2];
    FieldFx_DrawShadedRing(&workState->emitter, matrix, 0);
}

typedef struct RoomEruptionRiseScript {
    u8 state;
    u8 result;
    s16 frame;
} RoomEruptionRiseScript;

/* Grows the column and fades it after eight frames; the column's base hits
 * the owner when the owner stands inside its radius. */
void RoomEffect_GroundEruptionUpdateColumn(
    int unused, RoomEruptionRiseScript *script, RoomEruptionColumn *effect) {
    s16 position[3];
    RoomEruptionActor *actor;
    s16 value;

    actor = FieldFx_GetScriptContext();
    if (script->frame >= 9) {
        value = effect->depth;
        if (value >= 0x21) {
            effect->depth = value - 0x20;
        }
    }

    effect->scale.signedValue = effect->scale.unsignedValue +
        (effect->speed.signedValue >> 4);
    effect->speed.signedValue = effect->speed.unsignedValue + 0x320;
    if (script->frame == 0x10) {
        script->result = 2;
    }

    position[0] = actor->matrix.t[0];
    position[1] = g_RoomFloorY->y;
    position[2] = actor->matrix.t[2];
    if (FieldFx_IsPlayerWithinRadius(position, actor->collisionRadius) != 0) {
        actor->collisionState = 1;
    }
}

void RoomEffect_GroundEruptionInitGlow(void *unused0, void *unused1,
                                       RoomEruptionGlow *state) {
    RoomEruptionActor *obj = FieldFx_GetScriptContext();
    int tmp;
    int cst;

    tmp = obj->matrix.t[0];
    state->x = tmp;
    tmp = obj->matrix.t[1];
    state->y = tmp;
    tmp = obj->matrix.t[2];
    cst = 0x400;
    state->phase = 0;
    state->scale = cst;
    state->z = tmp;
}

/* The flash at the owner's feet. */
void RoomEffect_GroundEruptionDrawGlow(void) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomEruptionActor *owner;

    owner = FieldFx_GetScriptContext();
    FieldFx_SetSpriteTexturePreset(owner->asset.page);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);

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
    scale = s_EruptionGlowScale;
    ScaleMatrix(&matrix, &scale);

    matrix.t[0] = owner->matrix.t[0];
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = owner->matrix.t[2];
    FieldFx_DrawGlowSprite((FieldGlowSprite *)&g_RoomEruptionGlowPacket, &matrix, 1);
}

void RoomEffect_GroundEruptionUpdateGlow(void *unused, RoomFxControl *state,
                                         void *obj) {
    if (state->frame == 0x3A) {
        state->state = 2;
    }
}

void RoomEffect_GroundEruptionInitRing(void *arg0, void *arg1,
                                       RoomEruptionRing *ring) {
    RoomEruptionActor *owner;
    unsigned int i;
    int angle;

    owner = FieldFx_GetScriptContext();
    for (i = 0; i < 0x10; i++) {
        angle = i << 9;
        ring->point[i].x = owner->matrix.t[0];
        ring->point[i].y = owner->matrix.t[1] - 0x64;
        ring->point[i].z = owner->matrix.t[2];
        ring->direction[i].x = rsin(angle);
        ring->direction[i].pad02 = 0;
        ring->direction[i].z = rcos(angle);
    }

    ring->depth = 0x80;
    ring->radius = 0x400;
    ring->acceleration = -0x3C;
    ring->phase = 0;
    ring->speed = 0x320;
}

/* Draws the ring's centre sprite, then each point twice: in the air and
 * as its shadow on the floor. */
void RoomEffect_GroundEruptionDrawRing(void *arg0, void *arg1, RoomEruptionRing *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale1;
    RoomFxVec4 scale2;
    RoomEruptionActor *rawOwner;
    RoomEruptionActor *owner;
    unsigned int i;
    s16 *packetDepth;
    RoomFxSpritePacket *packet;
    RoomEruptionRing *workState;
    char *depthAddress;
    char *packetAddress;
    int ownerZ;
    u8 code;

    workState = state;
    rawOwner = FieldFx_GetScriptContext();
    i = 0;
    depthAddress = (char *)&g_RoomEruptionRingPacket.depth;
    PE1_COMPILER_USE(depthAddress);
    packetDepth = (s16 *)depthAddress;
    packetAddress = (char *)packetDepth - 10;
    PE1_COMPILER_USE(packetAddress);
    packet = (RoomFxSpritePacket *)packetAddress;
    owner = rawOwner;
    PE1_COMPILER_LAUNDER(owner);
    FieldFx_SetSpriteTexturePreset((s8)owner->asset.page);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);

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
    scale1 = s_EruptionRingScale;
    ScaleMatrix(&matrix, &scale1);
    matrix.t[0] = owner->matrix.t[0];
    matrix.t[1] = owner->matrix.t[1];
    ownerZ = owner->matrix.t[2];
    packet->depth = 0x40;
    matrix.t[2] = ownerZ;
    g_RoomEruptionRingPacket.code = workState->frame * 2 + 0x20;
    FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);

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
    scale2 = s_EruptionRingPointScale;
    ScaleMatrix(&matrix, &scale2);

    for (; i < 16; i++) {
        GteShortVector *point = &workState->point[i];
        matrix.t[0] = point->x;
        matrix.t[1] = point->y;
        matrix.t[2] = point->z;
        *packetDepth = (u16)workState->depth;
        code = workState->frame;
        RoomEffect_SpriteControlFromDepth(packetDepth)->code = code * 2 + 0x20;
        FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
        matrix.t[0] = point->x;
        matrix.t[1] = g_RoomFloorY->y;
        matrix.t[2] = point->z;
        *packetDepth = workState->depth >> 2;
        code = workState->frame;
        RoomEffect_SpriteControlFromDepth(packetDepth)->code = code * 2 + 0x20;
        FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
    }
}

/* Spreads the ring and fades it out over seven steps. */
void RoomEffect_GroundEruptionUpdateRing(void *arg0, unsigned char *signal,
                                         RoomEruptionRing *rec) {
    unsigned int i;
    GteShortVector *cur;
    unsigned char phase;
    int delta;

    rec->speed += rec->acceleration;
    if (rec->depth >= 0x11) {
        rec->depth -= 0x10;
    }

    for (i = 0; i < 0x10; i++) {
        cur = &rec->point[i];
        delta = (rec->direction[i].x * rec->speed) >> 16;
        cur->y += 8;
        cur->x += delta;
        delta = (rec->direction[i].z * rec->speed) >> 16;
        cur->z += delta;
    }

    phase = rec->phase + 6;
    rec->phase = phase;
    rec->frame = (phase >> 4) + 1;
    if ((phase >> 4) == 7) {
        rec->frame = 0;
        signal[1] = 2;
    }
}

void RoomEffect_GroundEruptionInitShadow(void *unused0, void *unused1,
                                         RoomEruptionShadow *state) {
    RoomEruptionActor *obj = FieldFx_GetScriptContext();
    unsigned int i;
    GteShortVector *entries = state->point;

    for (i = 0; i < 0x10; i++) {
        entries[i].x = obj->matrix.t[0];
        entries[i].y = obj->matrix.t[1];
        entries[i].z = obj->matrix.t[2];
    }

    state->depth = 0x80;
    state->scale = 0x800;
}

typedef struct RoomEruptionShadowStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomSpriteMatrix generatedMatrix;
    RoomFxVec4 scale;
} RoomEruptionShadowStack;

void RoomEffect_GroundEruptionDrawShadow(
    void *unused0, void *unused1, RoomFxGroundSpriteParams *fx) {
    RoomEruptionShadowStack stack;
    RoomEruptionActor *owner;
    unsigned short *depthSlot;
    RenderMatrixSlot *matrixSlot;

    owner = FieldFx_GetScriptContext();
    stack.seed = s_EruptionShadowSeed;
    RotMatrix(&stack.seed.vector, &stack.generatedMatrix);
    FieldFx_SetSpriteTexturePreset(owner->asset.page);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);

    stack.matrix.m[2][2] = 0x1000;
    stack.matrix.m[1][1] = 0x1000;
    stack.matrix.m[0][0] = 0x1000;
    stack.matrix.t[2] = 0;
    stack.matrix.t[1] = 0;
    stack.matrix.t[0] = 0;
    stack.matrix.m[2][1] = 0;
    stack.matrix.m[2][0] = 0;
    stack.matrix.m[1][2] = 0;
    stack.matrix.m[1][0] = 0;
    stack.matrix.m[0][2] = 0;
    stack.matrix.m[0][1] = 0;

    stack.scale = s_EruptionGlowScale;
    ScaleMatrix(&stack.matrix, &stack.scale);
    stack.matrix.t[0] = fx->x;
    stack.matrix.t[1] = g_RoomFloorY->y;
    stack.matrix.t[2] = fx->z;
    depthSlot = &g_RoomEruptionShadowPacket.depth;
    *depthSlot = fx->depth;
    matrixSlot = &g_ViewMatrixSlot;
    gte_ldrotmatrix(matrixSlot->value);
    gte_ldtransmatrix(matrixSlot->value);
    FieldFx_DrawGlowSprite((FieldGlowSprite *)((char *)depthSlot - 0xA), &stack.matrix, 1);
}

void RoomEffect_GroundEruptionNop2(void) {
}

/* Flings the six debris particles out at random angles. */
void RoomEffect_GroundEruptionInitDebris(void *arg0, void *arg1, RoomEruptionDebrisState *state) {
    RoomEruptionActor *owner;
    unsigned int i;
    int angle;
    int wave;

    owner = FieldFx_GetScriptContext();
    FieldFx_VarPtr(0);
    angle = rand() % 0x1000;
    for (i = 0; i < 6; i++) {
        state->position[i].x = owner->matrix.t[0];
        state->position[i].y = owner->matrix.t[1];
        state->position[i].z = owner->matrix.t[2];

        wave = rsin(angle);
        state->velocity[i].x =
            (wave * (rand() % 0x800 + 0x800)) >> 12;
        state->velocity[i].y = -(rand() % 6096 + 3000);
        state->velocity[i].z =
            (rcos(angle) * (rand() % 0x800 + 0x800)) >> 12;

        state->gravityStep[i] = 500;
        state->visible[i] = 1;
    }

    state->depth = 0x80;
    state->scale = 0x400;
    state->speed = 0x320;
}

/* Draws the live debris; the first particle also leaves a floor shadow. */
void RoomEffect_GroundEruptionDrawDebris(void *arg0, void *arg1, RoomEruptionDebrisState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomEruptionActor *owner;
    int depth;
    unsigned int i;
    GteShortVector *particle;
    RoomFxSpriteControl *packetFields;
    RoomFxSpritePacket *packet;
    u8 code;
    u8 mode;

    owner = FieldFx_GetScriptContext();
    depth = 0x80;
    i = 0;
    code = 0x42;
    mode = 3;
    packetFields = (RoomFxSpriteControl *)&g_RoomEruptionDebrisPacket.code;
    packet = (RoomFxSpritePacket *)((char *)packetFields - PE1_OFFSETOF(RoomFxSpritePacket, code));
    FieldFx_SetSpriteTexturePreset(owner->asset.page);
    FieldFx_SetSpriteCellSize(0x20, 0x20);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);
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
    scale = s_EruptionDebrisScale;
    ScaleMatrix(&matrix, &scale);

    for (; i < 6; i++) {
        particle = &state->position[i];
        depth -= 20;
        if (state->visible[i] == 1) {
            matrix.t[0] = particle->x;
            matrix.t[1] = particle->y;
            matrix.t[2] = particle->z;
            if (i == 0) {
                FieldFx_SetSpriteBlendMode(2);
                packetFields->code = code;
                packetFields->mode = mode;
                packetFields->depth = 0xFF;
            } else {
                FieldFx_SetSpriteBlendMode(3);
                packetFields->code = 0x47;
                packetFields->mode = 5;
                packetFields->depth = depth;
            }
            FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
            if (i == 0) {
                matrix.t[0] = state->position[0].x;
                matrix.t[1] = g_RoomFloorY->y;
                matrix.t[2] = state->position[0].z;
                g_RoomEruptionDebrisDepth = 0x40;
                g_RoomEruptionDebrisPacket.code = code;
                g_RoomEruptionDebrisPacket.mode = mode;
                FieldFx_SetSpriteBlendMode(2);
                FieldFx_DrawGlowSprite((FieldGlowSprite *)packet, &matrix, 1);
            }
        }
    }
}

typedef struct RoomEruptionTrailEntry {
    u32 word0;
    u32 word4;
} __attribute__((aligned(1), packed)) RoomEruptionTrailEntry;

typedef struct RoomEruptionTrailTrigger {
    u8 pad00;
    u8 state;
    s16 timer;
} RoomEruptionTrailTrigger;

typedef union RoomEruptionDebris {
    RoomEruptionDebrisState state;
    RoomEruptionTrailEntry history[6];
} RoomEruptionDebris;

/* Moves the lead debris particle under gravity and shifts its trail. */
void RoomEffect_GroundEruptionUpdateDebris(
    void *unused, RoomEruptionTrailTrigger *trigger, RoomEruptionDebris *object) {
    int count;

    count = 5;
    while (count != 0) {
        object->history[count] = object->history[count - 1];
        object->state.visible[count] = object->state.visible[count - 1];
        count--;
    }

    object->state.position[0].x += object->state.velocity[0].x >> 8;
    object->state.position[0].y += object->state.velocity[0].y >> 8;
    object->state.position[0].z += object->state.velocity[0].z >> 8;
    object->state.velocity[0].y += object->state.gravityStep[0];

    if (object->state.position[0].y > g_RoomFloorY->y) {
        object->state.visible[0] = 0;
    }
    if (trigger->timer == 60) {
        trigger->state = 2;
    }
}
