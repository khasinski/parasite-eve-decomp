/*
 * The twelve-element effect: a sprite class whose Init seeds two packet
 * templates, whose effect owner lays out twelve elements, draws each with
 * its floor shadow and ticks their counters. The eight functions, from the
 * two no-ops after RoomLib_CloseTarget to the counter tick in front of the
 * room library, are the same run in thirteen sewer, subway and Chrysler
 * rooms, and the unit's rodata (the three seeds) sits in front of the
 * library's jump tables in every one of them.
 */

#include "common.h"
#include "pe1/field_script_context.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte.h"
#include "pe1/psyq_libc.h"
#include "pe1/room_floor.h"
#include "pe1/room_fx.h"

#include "room_lib.h"

/* Seeds copied into the stack frames; file-scope so the all-zero rotation
 * stays a rodata copy. */
static const RoomFxSeed8 s_TwelveEffectInitSeed = {{0, 0, 0, 0, 0, 0, 0, 0}};
static const RoomFxSeed8 s_TwelveEffectDrawSeed = {{0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
static const RoomFxVec4 s_TwelveEffectShadowScale = {0x1000, 0x2000, 0x1000, 0};

int RoomLib_TwelveEffectNop0(void) {
    return 0;
}

int RoomLib_TwelveEffectNop1(void) {
    return 0;
}

/* Init: binds the state to the owner's matrix words and sets up the two
 * packet templates the draw pass fills (primary sprite, floor shadow). */
void RoomLib_InitTwelveEffectPackets(RoomEnt *obj, void *unused, RoomLibFxMatrixState *state) {
    RoomLink *owner;
    RoomLibFxMatrixWords *words;

    FieldFx_SetScriptContext(state);
    state->asset.index = Asset_SearchByKeyType(0xB);
    owner = obj->link;
    state->link = owner;
    words = owner->p238;
    state->matrix = *words;
    state->reload = 0x28;
    state->cooldown = 0;
    state->hit = 0;
    RoomLib_TwelveEffectPrimaryPacket.code = 4;
    RoomLib_TwelveEffectPrimaryPacket.mode = 1;
    RoomLib_TwelveEffectPrimaryPacket.offset = 0;
    RoomLib_TwelveEffectPrimaryPacket.depth = 0x80;
    RoomLib_TwelveEffectPrimaryPacket.r = 0x80;
    RoomLib_TwelveEffectPrimaryPacket.g = 0x80;
    RoomLib_TwelveEffectPrimaryPacket.b = 0x80;
    RoomLib_TwelveEffectPrimaryPacket.zero = 0;
    RoomLib_TwelveEffectSecondaryPacket.code = 8;
    RoomLib_TwelveEffectSecondaryPacket.mode = 2;
    RoomLib_TwelveEffectSecondaryPacket.offset = 0;
    RoomLib_TwelveEffectSecondaryPacket.depth = 0x30;
    RoomLib_TwelveEffectSecondaryPacket.r = 0x80;
    RoomLib_TwelveEffectSecondaryPacket.g = 0x80;
    RoomLib_TwelveEffectSecondaryPacket.b = 0x80;
    RoomLib_TwelveEffectSecondaryPacket.zero = 0;
}

void RoomLib_TwelveEffectNop3(void) {
}

void RoomLib_UpdateTwelveElementTimer(int arg0, char *arg1, char *arg2) {
    RoomLibFxMatrixState *state = (RoomLibFxMatrixState *)arg2;

    if (state->cooldown != 0) {
        state->cooldown--;
    }

    if (state->hit == 1) {
        int timer = state->cooldown;

        state->hit = 0;
        if (timer == 0) {
            state->cooldown = state->reload;
            FieldFx_RegisterOwnerContact(arg0);
        }
    }

    if (FieldFx_IsLastSlotHalted() == 1) {
        arg1[1] = 2;
    }
}

/* The element records followed, past the spawn work area, by the handle
 * of the effect the init hands to the field engine. */
typedef struct RoomTwelveEffectWork {
    RoomLibTick12Rec rec;
    u8 padBC[0x60];
    int effectHandle;             /* 0x11C */
} RoomTwelveEffectWork;

typedef struct RoomTwelveEffectStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix sourceMatrix;
    RoomSpriteMatrix localMatrix;
} RoomTwelveEffectStack;

int *FieldFx_VarPtr(int index);
int FieldFx_PlaySoundAtMatrixPosition(void *object, int message, void *state);

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_InitTwelveElementEffect(void *owner, void *unused, char *state) {
    RoomTwelveEffectStack stack;
    void *effectOwner;
    RoomLibTick12Rec *workState;
    int spacing;
    RoomSpriteMatrix *localMatrix;
    unsigned int i;
    int depth;
    int initialValue;
    RoomLibFxMatrixState *clock;
    register RoomSpriteMatrix *sourceMatrix asm("$3");

    effectOwner = owner;
    workState = (RoomLibTick12Rec *)state;
    clock = FieldFx_GetScriptContext();
    stack.seed = s_TwelveEffectInitSeed;
    stack.sourceMatrix = clock->matrix.matrix;

    workState->repeatCount = 0xC;
    workState->frameStep = *FieldFx_VarPtr(0);
    workState->timerLimit = *FieldFx_VarPtr(1);
    workState->initialPhase = *FieldFx_VarPtr(2);
    spacing = *FieldFx_VarPtr(3);
    *(s16 *)&stack.seed = *FieldFx_VarPtr(4);
    workState->phaseStep = *FieldFx_VarPtr(5);
    workState->phaseLimit = *FieldFx_VarPtr(6);

    localMatrix = &stack.localMatrix;
    localMatrix->m[2][2] = 0x1000;
    localMatrix->m[1][1] = 0x1000;
    localMatrix->m[0][0] = 0x1000;
    localMatrix->t[2] = 0;
    localMatrix->t[1] = 0;
    localMatrix->t[0] = 0;
    localMatrix->m[2][1] = 0;
    localMatrix->m[2][0] = 0;
    localMatrix->m[1][2] = 0;
    localMatrix->m[1][0] = 0;
    localMatrix->m[0][2] = 0;
    localMatrix->m[0][1] = 0;
    RotMatrix((GteShortVector *)&stack.seed, (GteMatrix *)localMatrix);

    sourceMatrix = &stack.sourceMatrix;
    gte_ldrotmatrix(sourceMatrix);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][0]);
    gte_stir123_column(sourceMatrix);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][1]);
    gte_stir123_column_at(&stack.sourceMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][2]);
    gte_stir123_column_at(&stack.sourceMatrix.m[0][2]);
    gte_ldtransmatrix(sourceMatrix);
    gte_ldv0_word3_at(localMatrix->t);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(stack.sourceMatrix.t);
    gte_swc2_10_4(stack.sourceMatrix.t);
    gte_swc2_11_8(stack.sourceMatrix.t);

    workState->matrix = stack.sourceMatrix;
    i = 0;
    initialValue = 0x64;
    depth = 0;
    for (; i < 12; i++) {
        workState->state[i] = 0;
        workState->timer[i] = 0;
        workState->phase[i] = initialValue;
        workState->position[i].z = depth;
        depth += spacing;
        workState->position[i].x = 0;
        workState->position[i].y = 0;
    }
    ((RoomTwelveEffectWork *)workState)->effectHandle =
        FieldFx_PlaySoundAtMatrixPosition(effectOwner, 0x58F, &stack.sourceMatrix);
}

typedef struct RoomTwelveEffectDrawStack {
    RoomSpriteMatrix primaryMatrix;
    RoomSpriteMatrix secondaryMatrix;
    RoomFxSeed8 seed;
    s16 collision[4];
    s16 point[4];
    RoomSpriteMatrix localMatrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomTwelveEffectDrawStack;

void ApplyMatrix(RoomSpriteMatrix *matrix, void *position, int *translation);
int FieldFx_IsPointInSpriteQuad(GteShortVector *point, GteMatrix *matrix);

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_DrawTwelveElementEffect(void *unused0, void *unused1,
                                     char *state) {
    RoomTwelveEffectDrawStack stack;
    RoomLibTick12Rec *workState;
    RoomLibFxMatrixState *clock;
    unsigned int i;
    RoomSpriteMatrix *primaryMatrix;
    RoomFxVec4 *sourceScale;
    RoomSpriteMatrix *secondaryMatrix;
    char *element;
    int pointX, pointY, pointZ;

    workState = (RoomLibTick12Rec *)state;
    clock = FieldFx_GetScriptContext();
    stack.seed = s_TwelveEffectDrawSeed;
    stack.localMatrix.m[2][2] = 0x1000;
    stack.localMatrix.m[1][1] = 0x1000;
    stack.localMatrix.m[0][0] = 0x1000;
    stack.localMatrix.t[2] = 0;
    stack.localMatrix.t[1] = 0;
    stack.localMatrix.t[0] = 0;
    stack.localMatrix.m[2][1] = 0;
    stack.localMatrix.m[2][0] = 0;
    stack.localMatrix.m[1][2] = 0;
    stack.localMatrix.m[1][0] = 0;
    stack.localMatrix.m[0][2] = 0;
    stack.localMatrix.m[0][1] = 0;
    RotMatrix((GteShortVector *)&stack.seed, (GteMatrix *)&stack.localMatrix);

    i = 0;
    primaryMatrix = &stack.primaryMatrix;
    sourceScale = &stack.sourceScale;
    secondaryMatrix = &stack.secondaryMatrix;
    FieldFx_SetSpriteTexturePreset(clock->asset.page);
    FieldFx_SetSpriteCellSize(0x40, 0x40);
    FieldFx_SetSpriteColorMode(0x10);
    FieldFx_SetSpriteBlendMode(2);

    for (; i < 12; i++) {
        element = &((char *)workState)[i];
        if (((RoomLibTick12Rec *)element)->state[0] == 1) {
            stack.primaryMatrix = workState->matrix;
            ApplyMatrix(primaryMatrix, &workState->position[i],
                          stack.primaryMatrix.t);

            memset(sourceScale, 0, 0x10);
            stack.sourceScale.x = (s16)workState->phase[i];
            stack.sourceScale.y = (s16)workState->phase[i];
            stack.sourceScale.z = (s16)workState->phase[i];
            stack.scale = stack.sourceScale;
            ScaleMatrix((GteMatrix *)primaryMatrix, (GteVector *)&stack.scale);

            stack.primaryMatrix.t[0] += clock->matrix.matrix.t[0];
            stack.primaryMatrix.t[1] += clock->matrix.matrix.t[1];
            stack.primaryMatrix.t[2] += clock->matrix.matrix.t[2];
            FieldFx_InterpolateColorRamp(RoomLib_TwelveEffectTable,
                          ((RoomLibTick12Rec *)element)->timer[0],
                          (unsigned char *)&RoomLib_TwelveEffectPrimaryPacket);

            stack.secondaryMatrix = stack.primaryMatrix;
            stack.sourceScale = s_TwelveEffectShadowScale;
            ScaleMatrix((GteMatrix *)secondaryMatrix, (GteVector *)sourceScale);

            gte_ldrotmatrix(secondaryMatrix);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][0]);
            gte_stir123_column(secondaryMatrix);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][1]);
            gte_stir123_column_at(&stack.secondaryMatrix.m[0][1]);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][2]);
            gte_stir123_column_at(&stack.secondaryMatrix.m[0][2]);
            gte_ldtransmatrix(secondaryMatrix);
            gte_ldv0_word3_at(stack.localMatrix.t);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtv0tr_sf0();
            gte_swc2_9_0(stack.secondaryMatrix.t);
            gte_swc2_10_4(stack.secondaryMatrix.t);
            gte_swc2_11_8(stack.secondaryMatrix.t);

            FieldFx_DrawGlowSprite((FieldGlowSprite *)&RoomLib_TwelveEffectPrimaryPacket,
                          primaryMatrix, 0);
            stack.secondaryMatrix.t[1] = g_RoomFloorY->y;
            FieldFx_InterpolateColorRamp(RoomLib_TwelveEffectTable,
                          ((RoomLibTick12Rec *)element)->timer[0],
                          (unsigned char *)&RoomLib_TwelveEffectSecondaryPacket);
            FieldFx_DrawGlowSprite((FieldGlowSprite *)&RoomLib_TwelveEffectSecondaryPacket,
                          secondaryMatrix, 0);

            stack.collision[0] = stack.secondaryMatrix.t[0];
            stack.collision[1] = 0;
            stack.collision[2] = stack.secondaryMatrix.t[2];
            if (Geo_PointInPoly(stack.secondaryMatrix.t[0] << 16,
                              stack.secondaryMatrix.t[2] << 16,
                              g_FloorPolygonVertices, g_FloorPolygonVertexCount) == 0) {
                ((RoomLibTick12Rec *)element)->state[0] = -1;
            }

            pointX = RW16(D_8009D254, 0x2A);
            stack.point[0] = pointX;
            pointY = RW16(D_8009D254, 0x2E);
            stack.point[1] = pointY;
            pointZ = RW16(D_8009D254, 0x32);
            stack.point[2] = pointZ;
            if (FieldFx_IsPointInSpriteQuad(stack.point, secondaryMatrix) != 0) {
                clock->hit = 1;
            }
        }
    }
}

ROOMLIB_TICK_12_COUNTERS(RoomLib_TickTwelveElementCounters)
