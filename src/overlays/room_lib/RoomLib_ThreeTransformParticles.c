/*
 * Three transform particles: a sprite class whose Init seeds the sprite and
 * floor-shadow packet templates, whose effect owner places three particles,
 * draws them with their shadows and lets them fall as debris. The eight
 * functions, from the two no-ops after RoomLib_ThreeParticleClose to the
 * debris update, are the same run in eleven Central Park and Chrysler
 * rooms; the unit's rodata is the draw pass's rotation seed, the eight
 * bytes after each room's twelve-byte rodata header.
 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/room_effect_state.h"
#include "pe1/field_script_context.h"
#include "pe1/gte_short_vector.h"
#include "pe1/room_floor.h"

extern void func_800C2B40(void *arg0);
extern void *func_8006DC18(int type);

int *func_800C2B28(int index);

/* Rotation seed of the draw pass, copied into its stack frame. */
static const RoomFxSeed8 s_ThreeParticleSeed = {
    {0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

int RoomLib_ThreeParticleNop0(void) {
    return 0;
}

int RoomLib_ThreeParticleNop1(void) {
    return 0;
}

void RoomLib_InitThreeParticlePackets(char *arg0, void *arg1, char *state) {
    RoomEffectWords8 *src;
    int *table;

    func_800C2B40(state);
    *(void **)(state + 0x2C) = func_8006DC18(0xA);
    table = *(int **)(arg0 + 8);
    *(int **)(state + 0x0) = table;
    src = *(RoomEffectWords8 **)((char *)table + 0x238);
    *(RoomEffectWords8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x26) = 0;
    *(s16 *)(state + 0x28) = 0;
    *(s16 *)(state + 0x24) = *func_800C2B28(6);

    RoomLib_ThreeParticlePacket.offset = -0x64;
    RoomLib_ThreeParticleFloorPacket.code = 4;
    RoomLib_ThreeParticleFloorPacket.mode = 1;
    RoomLib_ThreeParticlePacket.code = 0;
    RoomLib_ThreeParticlePacket.mode = 0;
    RoomLib_ThreeParticlePacket.depth = 0x80;
    RoomLib_ThreeParticlePacket.r = 0x80;
    RoomLib_ThreeParticlePacket.g = 0x80;
    RoomLib_ThreeParticlePacket.b = 0x80;
    RoomLib_ThreeParticlePacket.zero = 0;
    RoomLib_ThreeParticleFloorPacket.offset = 0x32;
    RoomLib_ThreeParticleFloorPacket.depth = 0x80;
    RoomLib_ThreeParticleFloorPacket.r = 0x80;
    RoomLib_ThreeParticleFloorPacket.g = 0x80;
    RoomLib_ThreeParticleFloorPacket.b = 0x80;
    RoomLib_ThreeParticleFloorPacket.zero = 0;
}

void RoomLib_ThreeParticleNop3(void) {
}

extern void func_800C6C18(int arg0);
extern int func_800C2B68(void);

void RoomLib_UpdateThreeParticleTimer(int arg0, char *arg1, char *arg2) {
    short *state = (short *)arg2;

    if (state[0x13] != 0) {
        state[0x13]--;
    }

    if (state[0x14] == 1) {
        int timer = state[0x13];

        state[0x14] = 0;
        if (timer == 0) {
            state[0x13] = *(unsigned short *)(arg2 + 0x24);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}

typedef GteShortVector RoomInitThreeVec;

typedef struct RoomInitThreeStack {
    RoomInitThreeVec projectionSeed;
    RoomInitThreeVec firstSeed;
    RoomInitThreeVec secondSeed;
    RoomSpriteMatrix firstMatrix;
    RoomSpriteMatrix secondMatrix;
} RoomInitThreeStack;

int *func_800C2B10(int index);
int func_80071A54(void);
void func_800C66C8(void *owner, int id, void *state);

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_InitThreeTransformParticles(void *owner, void *unused,
                                         char *state) {
    RoomInitThreeStack stack;
    RoomSpriteMatrix *firstMatrix;
    register RoomSpriteMatrix *secondMatrix asm("$17");
    RoomInitThreeVec *seed;
    void *callOwner;
    register char *workState asm("$21");
    char *clock;
    int i;
    register char *byteCursor asm("$16");
    register char *positionCursor asm("$17");
    register char *velocityCursor asm("$18");
    register char *timerCursor asm("$19");
    int parameterIndex;
    char *clockResult;
    int *value;
    int randomValue;

    asm("" : "=r"(callOwner), "=r"(workState) : "0"(owner), "1"(state));
    clockResult = func_800C2B50();
    parameterIndex = 3;
    clock = clockResult;
        stack.firstSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.firstSeed.y = -*value;
    stack.firstSeed.z = 0;
    seed = &stack.firstSeed;
    asm("" : : "r"(seed));
    firstMatrix = &stack.firstMatrix;
    RotMatrixYXZ(seed, firstMatrix);

    parameterIndex = 3;
    stack.secondSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.secondSeed.y = *value;
    stack.secondSeed.z = 0;
    seed = &stack.secondSeed;
    asm("" : : "r"(seed));
    secondMatrix = &stack.secondMatrix;
    RotMatrixYXZ(seed, secondMatrix);

    gte_ldrotmatrix(firstMatrix);
    gte_ldrtir12_matrix_column(clock + 4);
    gte_stir123_column(firstMatrix);
    gte_ldrtir12_matrix_column(clock + 6);
    gte_stir123_column_at(&stack.firstMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(clock + 8);
    gte_stir123_column_at(&stack.firstMatrix.m[0][2]);
    gte_ldtransmatrix(firstMatrix);
    {
        unsigned short *translation = (unsigned short *)(clock + 0x18);
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(stack.firstMatrix.t);
        gte_swc2_10_4(stack.firstMatrix.t);
        gte_swc2_11_8(stack.firstMatrix.t);

        gte_ldrotmatrix(secondMatrix);
        gte_ldrtir12_matrix_column(clock + 4);
        gte_stir123_column(secondMatrix);
        gte_ldrtir12_matrix_column(clock + 6);
        gte_stir123_column_at(&stack.secondMatrix.m[0][1]);
        gte_ldrtir12_matrix_column(clock + 8);
        gte_stir123_column_at(&stack.secondMatrix.m[0][2]);
        gte_ldtransmatrix(secondMatrix);
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(stack.secondMatrix.t);
        gte_swc2_10_4(stack.secondMatrix.t);
        gte_swc2_11_8(stack.secondMatrix.t);
    }

    i = 0;
    velocityCursor = workState;
    positionCursor = workState;
    timerCursor = workState;
    do {
        byteCursor = workState + i;
        randomValue = func_80071A54();
        *(u8 *)(byteCursor + 0x1E) = 0;
        *(u8 *)(byteCursor + 0x1A) = (randomValue % 10) + 1;
        *(s16 *)(timerCursor + 0x22) = *func_800C2B28(2);
        *(s32 *)(positionCursor + 0x3C) = *(s32 *)(clock + 0x18) << 16;
        *(s32 *)(positionCursor + 0x40) = *(s32 *)(clock + 0x1C) << 16;
        *(s32 *)(positionCursor + 0x44) = *(s32 *)(clock + 0x20) << 16;
        *(s16 *)(velocityCursor + 0x6C) = 0;
        *(s16 *)(velocityCursor + 0x6E) = 0;
        *(s16 *)(velocityCursor + 0x70) = 0;
        *(s16 *)(timerCursor + 0x2A) = 0;
        *(u8 *)(byteCursor + 0x0E) = 1;
        *(u8 *)(byteCursor + 0x16) = 0;
        *(u8 *)(byteCursor + 0x12) = 0;
        i++;
        velocityCursor += 8;
        positionCursor += 0x10;
        timerCursor += 2;
    } while (i < 3);

    *(s16 *)(workState + 0x00) = 3;
    *(s16 *)(workState + 0x02) = *func_800C2B28(0);
    *(s16 *)(workState + 0x04) = *func_800C2B28(1);
    *(s16 *)(workState + 0x06) = *func_800C2B28(5);
    *(u8 *)(workState + 0x0A) = *func_800C2B28(4);
    *(s16 *)(workState + 0x08) = *func_800C2B10(1);

    stack.projectionSeed.x = 0;
    stack.projectionSeed.y = 0;
    stack.projectionSeed.z = 0x1000;
    ApplyMatrixSV(&stack.firstMatrix, &stack.projectionSeed,
                  (GteShortVector *)(workState + 0x9C));
    byteCursor = clock + 4;
    ApplyMatrixSV((RoomSpriteMatrix *)byteCursor, &stack.projectionSeed,
                  (GteShortVector *)(workState + 0xA4));
    ApplyMatrixSV(&stack.secondMatrix, &stack.projectionSeed,
                  (GteShortVector *)(workState + 0xAC));
    func_800C66C8(callOwner, 0x573, byteCursor);
}

typedef GteShortVector RoomDrawThreeVec;

typedef struct RoomDrawThreeStack {
    RoomSpriteMatrix primaryMatrix;
    RoomSpriteMatrix secondaryMatrix;
    RoomFxSeed8 seed;
    RoomDrawThreeVec position;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
    RoomFxVec4 floorScale;
    RoomFxVec4 sourceFloorScale;
    s16 *args;
} RoomDrawThreeStack;

void func_800C2EAC(u8 owner);
void func_800C3098(int depth);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_800C3134(void *table, int step, void *packet);
void func_800C2FF0(int width, int height);
void func_800C3238(int mode);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
int rsin(int angle);


void RoomLib_DrawThreeTransformParticles(void *unused, s16 *args, char *state) {
    RoomDrawThreeStack stack;
    register char *workState asm("$18");
    unsigned int i;
    RoomFxVec4 *sourceScale;
    RoomSpriteMatrix *secondaryMatrix;
    register char *positionCursor asm("$19");
    register char *shortCursor asm("$17");
    register char *element asm("$21");
    register int positionOffset asm("$22");
    register int scaleValue asm("$16");
    register void *arg0 asm("$4");
    void *inputArg;
    register void *outputArg asm("$6");
    register s16 *callArgs asm("$7");
    register char *positionSource asm("$2");
    char *clock;
    int computedScale;
    int baseScale;

    asm("" : "=r"(workState) : "0"(state));
    stack.args = args;
    clock = func_800C2B50();
    stack.seed = s_ThreeParticleSeed;
    i = 0;
    sourceScale = &stack.sourceScale;
    secondaryMatrix = &stack.secondaryMatrix;
    asm("" : "=r"(sourceScale), "=r"(secondaryMatrix)
        : "0"(sourceScale), "1"(secondaryMatrix));
    positionCursor = workState;
    shortCursor = workState;
    func_800C2EAC(*(u8 *)(clock + 0x2C));
    func_800C3098(0x10);

    do {
        element = workState + i;
        positionOffset = i << 3;
        if (*(s8 *)(element + 0x16) >= 0) {
            arg0 = (void *)(positionOffset + 0x6C);
            arg0 = workState + (int)arg0;
            RotMatrixYXZ(arg0, &stack.primaryMatrix);
            func_80071A44(sourceScale, 0, 0x10);
            stack.sourceScale.x = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.sourceScale.y = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.sourceScale.z = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.scale = stack.sourceScale;
            ScaleMatrix(&stack.primaryMatrix, &stack.scale);

            stack.primaryMatrix.t[0] = *(s16 *)(positionCursor + 0x3E) +
                                       *(s8 *)(workState + 0x0B);
            stack.primaryMatrix.t[1] = *(s16 *)(positionCursor + 0x42) +
                                       *(s8 *)(workState + 0x0C);
            stack.primaryMatrix.t[2] = *(s16 *)(positionCursor + 0x46) +
                                       *(s8 *)(workState + 0x0D);
            callArgs = stack.args;
            computedScale = *(u8 *)(element + 0x1E);
            arg0 = RoomLib_ThreeParticleTable;
            RoomLib_ThreeParticleAlpha = computedScale;
            asm volatile("" ::: "memory");
            inputArg = (void *)(int)callArgs[1];
            outputArg = (unsigned char *)&RoomLib_ThreeParticlePacket;
            func_800C3134(arg0, (int)inputArg, outputArg);
            func_800C2FF0(0x40, 0x40);
            func_800C3238(2);
            arg0 = (unsigned char *)&RoomLib_ThreeParticlePacket;
            func_800C42A4(arg0, &stack.primaryMatrix, 1);

            if (*(s16 *)(workState + 8) == 1) {
                callArgs = stack.args;
                computedScale = rsin(callArgs[1] << 7) >> 3;
                baseScale = *(u16 *)(shortCursor + 0x32);
                computedScale += baseScale;
                scaleValue = computedScale;
                asm("" : "=r"(scaleValue) : "0"(scaleValue));
                if (*(u16 *)(shortCursor + 0x2A) < (s16)computedScale) {
                    scaleValue = *(u16 *)(shortCursor + 0x2A);
                }

                positionSource =
                    (char *)((int)positionOffset + (int)workState);
                stack.position = *(RoomDrawThreeVec *)(positionSource + 0x6C);
                arg0 = &stack.position;
                inputArg = &stack.primaryMatrix;
                asm("" : "=r"(arg0), "=r"(inputArg)
                    : "0"(arg0), "1"(inputArg));
                scaleValue = (s16)scaleValue >> 1;
                stack.position.z += *(u16 *)(shortCursor + 0x22) << 4;
                RotMatrixYXZ(arg0, inputArg);
                func_80071A44(sourceScale, 0, 0x10);
                stack.sourceScale.x = scaleValue;
                stack.sourceScale.y = scaleValue;
                stack.sourceScale.z = scaleValue;
                ScaleMatrix(&stack.primaryMatrix, sourceScale);
                stack.primaryMatrix.t[0] = *(s16 *)(positionCursor + 0x3E) +
                                           *(s8 *)(workState + 0x0B);
                stack.primaryMatrix.t[1] = *(s16 *)(positionCursor + 0x42) +
                                           *(s8 *)(workState + 0x0C);
                stack.primaryMatrix.t[2] = *(s16 *)(positionCursor + 0x46) +
                                           *(s8 *)(workState + 0x0D);
                arg0 = (unsigned char *)&RoomLib_ThreeParticlePacket;
                func_800C42A4(arg0, &stack.primaryMatrix, 1);

                RotMatrix(&stack.seed.vector, secondaryMatrix);
                func_80071A44(&stack.sourceFloorScale, 0, 0x10);
                stack.sourceFloorScale.x = *(u16 *)(shortCursor + 0x2A);
                stack.sourceFloorScale.y = *(u16 *)(shortCursor + 0x2A);
                stack.sourceFloorScale.z = *(u16 *)(shortCursor + 0x2A);
                stack.floorScale = stack.sourceFloorScale;
                ScaleMatrix(secondaryMatrix, &stack.floorScale);
                computedScale = *(s16 *)(positionCursor + 0x3E);
                baseScale = g_RoomFloorY->y;
                                arg0 = (void *)0x20;
                stack.secondaryMatrix.t[0] = computedScale;
                stack.secondaryMatrix.t[1] = baseScale;
                computedScale = *(s16 *)(positionCursor + 0x46);
                asm volatile("" : "=r"(computedScale)
                    : "0"(computedScale));
                inputArg = (void *)0x20;
                asm("" : "=r"(inputArg) : "0"(inputArg));
                stack.secondaryMatrix.t[2] = computedScale;
                computedScale = *(u8 *)(element + 0x1E);
                scaleValue = (int)&RoomLib_ThreeParticleFloorPacket.depth;
                *(s16 *)scaleValue = (unsigned int)computedScale >> 1;
                func_800C2FF0((int)arg0, (int)inputArg);
                func_800C3238(3);
                func_800C42A4((char *)scaleValue - 0xA,
                              secondaryMatrix, 0);
            }
        }
        positionCursor += 0x10;
        i++;
        shortCursor += 2;
    } while (i < 3);
}

/* 16.16 fixed-point position; the integer halves are read as `>> 16`,
 * which the compiler narrows to halfword loads as retail does. */
typedef struct RoomDebrisPosition {
    s32 x, y, z;
    s32 reserved0C;
} RoomDebrisPosition;

typedef struct RoomDebrisVelocity {
    s16 x, y, z, reserved06;
} RoomDebrisVelocity;

/* Three pieces of debris that fall, bounce on the floor, and fade. */
typedef struct RoomDebrisTriplet {
    s16 remaining;                /* 0x00 */
    s16 speed;                    /* 0x02 */
    s16 acceleration;             /* 0x04 */
    s16 swingLimit;               /* 0x06 */
    s16 reserved08;
    u8 keepAlive;                 /* 0x0A */
    s8 jitter[3];                 /* 0x0B */
    u8 mode[4];                   /* 0x0E */
    u8 landed[4];                 /* 0x12 */
    s8 active[4];                 /* 0x16 */
    s8 countdown[4];              /* 0x1A */
    u8 size[4];                   /* 0x1E */
    u16 life[4];                  /* 0x22 */
    u16 swing[4];                 /* 0x2A */
    u16 phase[4];                 /* 0x32 */
    s16 reserved3A;
    RoomDebrisPosition position[4]; /* 0x3C */
    RoomDebrisVelocity reserved7C[4];
    RoomDebrisVelocity velocity[4]; /* 0x9C */
} RoomDebrisTriplet;

typedef struct RoomDebrisOwner {
    u8 reserved00;
    u8 phase;                     /* 0x01 */
    s16 angle;                    /* 0x02 */
} RoomDebrisOwner;

typedef struct RoomDebrisClock {
    u8 reserved[0x28];
    s16 landedFlag;               /* 0x28 */
} RoomDebrisClock;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, position) == 0x3C,
                  room_debris_triplet_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, velocity) == 0x9C,
                  room_debris_triplet_velocity_offset);

extern void *D_8009D248;
extern u16 D_8009D1CC;
extern int func_80071A54(void);
extern int rsin(int);
extern int func_8001CAB0(int, int, void *, int);
extern int func_800C6B90(void *position, int radius);

void RoomLib_UpdateDebrisTriplet(void *unused, RoomDebrisOwner *owner,
                                        RoomDebrisTriplet *state) {
    s16 point[4];
    RoomDebrisClock *clock;
    unsigned int i;
    int step;

    clock = func_800C2B50();
    for (i = 0; i < 3; i++) {
        if (state->countdown[i] != 0) {
            state->countdown[i]--;
            if (state->countdown[i] == 0) state->active[i] = 0;
        }
        if (state->active[i] == 0) {
            if (state->size[i] < 0x80) {
                state->size[i] += 4;
            } else {
                state->active[i] = 1;
            }
        }
        if (state->mode[i] == 1) {
            state->position[i].x += (state->velocity[i].x * state->speed) >> 4;
            state->position[i].y += (state->velocity[i].y * state->speed) >> 4;
            state->position[i].z += (state->velocity[i].z * state->speed) >> 4;
        }
        if (state->swing[i] < state->swingLimit) {
            state->swing[i] += func_80071A54() % 80 + 20;
            state->phase[i] += func_80071A54() % 40 + 40;
            state->position[i].y += -0xA0000;
        }
        state->swing[i] += rsin(owner->angle << 5) >> 7;
        if (state->life[i] < 0x14) {
            if (state->size[i] >= 7) state->size[i] -= 6;
        }
        if (state->landed[i] == 0) {
            if (!func_8001CAB0((state->position[i].x >> 16) << 16,
                               (state->position[i].z >> 16) << 16,
                               D_8009D248, D_8009D1CC)) {
                state->landed[i] = 1;
                state->mode[i] = 0;
                if (state->keepAlive == 0) state->life[i] = 0x14;
            }
        }
        state->life[i]--;
        if (state->life[i] == 0) {
            state->active[i] = -1;
            state->remaining--;
        }
        point[0] = (state->position[i].x >> 16);
        point[1] = (state->position[i].y >> 16);
        point[2] = (state->position[i].z >> 16);
        state->jitter[0] = func_80071A54() % 11 - 5;
        state->jitter[1] = func_80071A54() % 11 - 5;
        state->jitter[2] = func_80071A54() % 11 - 5;
        if (func_800C6B90(point, state->swing[i] >> 4)) {
            if (state->landed[i] == 0) {
                clock->landedFlag = 1;
                state->life[i] = 0x14;
                state->mode[i] = 0;
            }
            state->landed[i] = 1;
        }
    }
    state->speed += state->acceleration;
    if ((s16)state->speed < 0) state->speed = 0;
    if (state->remaining == 0) owner->phase = 2;
}
