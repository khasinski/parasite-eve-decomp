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
#include "room_lib.h"
#include "RoomLib_RenderLayouts.h"
#include "pe1/pe_image.h"
#include "pe1/random.h"
#include "pe1/room_ground_eruption.h"

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
void RoomEffect_GroundEruptionInit(char *owner, void *arg1, char *state) {
    char *entity;
    int asset;
    register char *callOwner asm("$4");
    int depth;
    int color;
    int offset;
    int modeB;
    char *workState;

    asm("" : "=r"(workState) : "0"(state));
    func_800C2B40(workState);
    *(s16 *)(workState + 0x2A) = 0;
    *(s16 *)(workState + 0x2C) = 0;
    *(s16 *)(workState + 0x28) = *func_800C2B28(5);
    *(s16 *)(workState + 0x2E) = *func_800C2B28(4);
    entity = *(char **)(owner + 8);
    *(char **)workState = entity;
    *(RoomSpriteMatrix *)(workState + 4) =
        **(RoomSpriteMatrix **)(entity + 0x238);
    *(s32 *)(workState + 0x18) = *func_800C2B28(1);
    *(s32 *)(workState + 0x1C) = *func_800C2B28(2);
    *(s32 *)(workState + 0x20) = *func_800C2B28(3);
    asset = Asset_SearchByKeyType(0xA6);
    callOwner = owner;
    *(int *)(workState + 0x24) = asset;

    g_RoomEruptionGlowPacket.code = 0x42;
    g_RoomEruptionGlowPacket.mode = 3;
    {
        int minus50;
        asm volatile("" : "=r"(minus50), "=r"(depth), "=r"(color)
                     : "0"(-50), "1"(0x80), "2"(0x80));
        g_RoomEruptionGlowPacket.offset = minus50;
    }
    g_RoomEruptionRingPacket.code = 0x20;
    g_RoomEruptionRingPacket.mode = 1;
    g_RoomEruptionRingPacket.offset = -51;
    g_RoomEruptionShadowPacket.code = 0x40;
    {
        int two;
        asm volatile("" : "=r"(two), "=r"(offset)
                     : "0"(2), "1"(-41));
        g_RoomEruptionShadowPacket.mode = two;
    }
    g_RoomEruptionDebrisPacket.code = 0x47;
    asm volatile("" : "=r"(modeB) : "0"(5));
    PE1_COMPILER_LAUNDER(workState);
    workState += 4;
    PE1_COMPILER_LAUNDER(workState);
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
    D_800942EC = 0;
    func_800C66C8(callOwner, 0x587, workState);
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
            if (FieldEng_GetStatus(ent) == 3) {
                if (ent->link->target->state[0] == 2) {
                    reload = work->h28;
                    flagsBase = (int **)D_8009D254;
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

    if (func_800C2B68() == 1) {
        signal[1] = 2;
    }
}

/* Emitter parameters of the rising column. */
void RoomEffect_GroundEruptionInitColumn(void *ent, void *unused,
                                         unsigned char *state) {
    *(short *)(state + 0x120) = 0x5DC;
    *(short *)(state + 0x122) = 0xFF;
    *(short *)(state + 0x114) = 0x10;
    state[0x10C] = 0x40;
    state[0x10D] = 0x20;
    state[0x10E] = 0x10;
    state[0x110] = 0x80;
    state[0x111] = 0x80;
    state[0x112] = 0x80;
    *(short *)(state + 0x116) = 0xF0;
    *(short *)(state + 0x118) = 0xA;
    *(short *)(state + 0x124) = 0;
    *(short *)(state + 0x11A) = 0;
    *(void **)(state + 0x108) = state + 8;
    func_800C4E50(state + 0x108);
}

/* The retail function is marked handwritten and uses four explicit GTE windows. */
void RoomEffect_GroundEruptionTransformColumn(void *arg0, void *arg1,
                                              char *state) {
    RoomLibSpriteTransformStack stack;
    register RoomSpriteMatrix *matrix asm("$16");
    RoomSpriteMatrix *scaleMatrix;
    char *workState;
    char *owner;

    asm("" : "=r"(workState) : "0"(state));
    owner = func_800C2B50();
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
    func_80071A44(&stack.sourceScale, 0, 0x10);

    stack.sourceScale.x = *(s16 *)(workState + 0x120);
    stack.sourceScale.y = *(s16 *)(workState + 0x120);
    stack.sourceScale.z = *(s16 *)(workState + 0x120);
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    ScaleMatrix(scaleMatrix, &stack.scale);
    {
        register GteShortVector *seedArg asm("$4") = &stack.seed.vector;
        asm("" : : "r"(seedArg));
        matrix = &stack.matrix;
        RotMatrix(seedArg, matrix);
    }

    gte_ldrotmatrix(scaleMatrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][0]);
    gte_stir123_column(matrix);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][1]);
    gte_stir123_column_at(&stack.matrix.m[0][1]);
    gte_ldrtir12_matrix_column(&stack.matrix.m[0][2]);
    gte_stir123_column_at(&stack.matrix.m[0][2]);
    gte_ldtransmatrix(scaleMatrix);
    {
        unsigned short *translation = (unsigned short *)stack.matrix.t;
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(translation);
        gte_swc2_10_4(translation);
        gte_swc2_11_8(translation);
    }

    func_800C3238(2);
    *(s16 *)(workState + 0x11C) = *(u16 *)(workState + 0x122);
    stack.matrix.t[0] = *(int *)(owner + 0x18);
    stack.matrix.t[1] = *(int *)(owner + 0x1C);
    stack.matrix.t[2] = *(int *)(owner + 0x20);
    func_800C4FC4(workState + 0x108, matrix, 0);
}

typedef struct RoomEruptionRiseScript {
    u8 state;
    u8 result;
    s16 frame;
} RoomEruptionRiseScript;

typedef struct RoomEruptionRise {
    unsigned char pad00[0x120];
    union {
        u16 unsignedValue;
        s16 signedValue;
    } horizontal;
    s16 vertical;
    union {
        u16 unsignedValue;
        s16 signedValue;
    } speed;
} RoomEruptionRise;

typedef struct RoomEruptionActor {
    unsigned char pad00[0x18];
    s32 x;
    s32 y;
    s32 z;
    unsigned char owner;
    unsigned char pad25[7];
    s16 collisionState;
    s16 collisionRadius;
} RoomEruptionActor;

/* Grows the column and fades it after eight frames; the column's base hits
 * the owner when the owner stands inside its radius. */
void RoomEffect_GroundEruptionUpdateColumn(
    int unused, RoomEruptionRiseScript *script, RoomEruptionRise *effect) {
    s16 position[3];
    RoomEruptionActor *actor;
    s16 value;

    actor = func_800C2B50();
    if (script->frame >= 9) {
        value = effect->vertical;
        if (value >= 0x21) {
            effect->vertical = value - 0x20;
        }
    }

    effect->horizontal.signedValue = effect->horizontal.unsignedValue +
        (effect->speed.signedValue >> 4);
    effect->speed.signedValue = effect->speed.unsignedValue + 0x320;
    if (script->frame == 0x10) {
        script->result = 2;
    }

    position[0] = actor->x;
    position[1] = D_800942EC;
    position[2] = actor->z;
    if (func_800C6B90(position, actor->collisionRadius) != 0) {
        actor->collisionState = 1;
    }
}

void RoomEffect_GroundEruptionInitGlow(void *unused0, void *unused1,
                                       char *state) {
    char *obj = (char *)func_800C2B50();
    int tmp;
    int cst;

    tmp = RW32(obj, 0x18);
    RW16(state, 0) = tmp;
    tmp = RW32(obj, 0x1C);
    RW16(state, 2) = tmp;
    tmp = RW32(obj, 0x20);
    cst = 0x400;
    RW16(state, 0xA) = 0;
    RW16(state, 8) = cst;
    RW16(state, 4) = tmp;
}

/* The flash at the owner's feet. */
void RoomEffect_GroundEruptionDrawGlow(void) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomEruptionActor *owner;

    owner = func_800C2B50();
    func_800C2EAC(owner->owner);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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

    matrix.t[0] = owner->x;
    matrix.t[1] = D_800942EC;
    matrix.t[2] = owner->z;
    func_800C42A4(&g_RoomEruptionGlowPacket, &matrix, 1);
}

void RoomEffect_GroundEruptionUpdateGlow(void *unused, char *state,
                                         void *obj) {
    if (RW16(state, 2) == 0x3A) {
        state[1] = 2;
    }
}

typedef struct RoomEruptionRingDirection {
    s16 x;
    s16 pad02;
    s16 z;
    s16 pad06;
} RoomEruptionRingDirection;

/* The shock ring: sixteen points around the owner, each moving outwards
 * along its own direction. */
typedef struct RoomEruptionRing {
    GteShortVector point[16];
    RoomEruptionRingDirection direction[16];
    s16 radius;
    s16 height;
    u8 frame;
    u8 phase;
    s16 speed;
    s16 acceleration;
} RoomEruptionRing;

void RoomEffect_GroundEruptionInitRing(void *arg0, void *arg1,
                                       RoomEruptionRing *ring) {
    RoomEruptionActor *owner;
    unsigned int i;
    int angle;

    owner = func_800C2B50();
    for (i = 0; i < 0x10; i++) {
        angle = i << 9;
        ring->point[i].x = owner->x;
        ring->point[i].y = owner->y - 0x64;
        ring->point[i].z = owner->z;
        ring->direction[i].x = rsin(angle);
        ring->direction[i].pad02 = 0;
        ring->direction[i].z = rcos(angle);
    }

    ring->height = 0x80;
    ring->radius = 0x400;
    ring->acceleration = -0x3C;
    ring->phase = 0;
    ring->speed = 0x320;
}

/* Draws the ring's centre sprite, then each point twice: in the air and
 * as its shadow on the floor. */
void RoomEffect_GroundEruptionDrawRing(void *arg0, void *arg1, char *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale1;
    RoomFxVec4 scale2;
    char *rawOwner;
    register char *owner asm("$17");
    register unsigned int i asm("$21");
    register GteShortVector *point asm("$18");
    s16 *packetDepth;
    RoomFxSpritePacket *packet;
    char *workState;
    register char *packetTemp asm("$3");
    int ownerZ;
    u8 code;

    asm("" : "=r"(workState) : "0"(state));
    rawOwner = func_800C2B50();
    i = 0;
    PE1_COMPILER_USE(i);
    packetTemp = (char *)&g_RoomEruptionRingPacket.depth;
    PE1_COMPILER_USE(packetTemp);
    packetDepth = (s16 *)packetTemp;
    PE1_COMPILER_USE(packetDepth);
    packetTemp = (char *)packetDepth - 10;
    PE1_COMPILER_USE(packetTemp);
    packet = (RoomFxSpritePacket *)packetTemp;
    PE1_COMPILER_USE(packet);
    owner = rawOwner;
    PE1_COMPILER_USE(owner);
    point = (GteShortVector *)workState;
    func_800C2EAC(owner[0x24]);
    PE1_COMPILER_USE(point);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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
    matrix.t[0] = *(int *)(owner + 0x18);
    matrix.t[1] = *(int *)(owner + 0x1C);
    ownerZ = *(int *)(owner + 0x20);
    packet->depth = 0x40;
    matrix.t[2] = ownerZ;
    g_RoomEruptionRingPacket.code = *(u8 *)(workState + 0x104) * 2 + 0x20;
    func_800C42A4(packet, &matrix, 1);

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

    do {
        matrix.t[0] = point->x;
        matrix.t[1] = point->y;
        matrix.t[2] = point->z;
        *packetDepth = *(u16 *)(workState + 0x102);
        code = *(u8 *)(workState + 0x104);
        asm("" : "=r"(i) : "0"(i), "r"(code));
        i++;
        *((u8 *)packetDepth - 6) = code * 2 + 0x20;
        func_800C42A4(packet, &matrix, 1);
        matrix.t[0] = point->x;
        matrix.t[1] = D_800942EC;
        matrix.t[2] = point->z;
        *packetDepth = (s16)*(u16 *)(workState + 0x102) >> 2;
        code = *(u8 *)(workState + 0x104);
        point++;
        *((u8 *)packetDepth - 6) = code * 2 + 0x20;
        func_800C42A4(packet, &matrix, 1);
    } while (i < 16);
}

/* Spreads the ring and fades it out over seven steps. */
void RoomEffect_GroundEruptionUpdateRing(void *arg0, unsigned char *signal,
                                         char *rec) {
    unsigned int i;
    RoomFxTrajectoryParticle *cur;
    unsigned char phase;
    int delta;

    *(unsigned short *)(rec + 0x106) += *(unsigned short *)(rec + 0x108);
    if (*(short *)(rec + 0x102) >= 0x11) {
        *(unsigned short *)(rec + 0x102) -= 0x10;
    }

    for (i = 0; i < 0x10; i++) {
        cur = (RoomFxTrajectoryParticle *)(rec + (i * sizeof(RoomFxTrajectoryParticle)));
        delta = (*(short *)((char *)cur + 0x80) * *(short *)(rec + 0x106)) >> 16;
        cur->y += 8;
        cur->x += delta;
        delta = (*(short *)((char *)cur + 0x84) * *(short *)(rec + 0x106)) >> 16;
        cur->z += delta;
    }

    phase = *(unsigned char *)(rec + 0x105) + 6;
    *(unsigned char *)(rec + 0x105) = phase;
    *(unsigned char *)(rec + 0x104) = (phase >> 4) + 1;
    if ((phase >> 4) == 7) {
        *(unsigned char *)(rec + 0x104) = 0;
        signal[1] = 2;
    }
}

void RoomEffect_GroundEruptionInitShadow(void *unused0, void *unused1,
                                         char *state) {
    char *obj = (char *)func_800C2B50();
    unsigned int i;
    GteShortVector *entries = (GteShortVector *)state;

    for (i = 0; i < 0x10; i++) {
        entries[i].x = RW32(obj, 0x18);
        entries[i].y = RW32(obj, 0x1C);
        entries[i].z = RW32(obj, 0x20);
    }

    RW16(state, 0x82) = 0x80;
    RW16(state, 0x80) = 0x800;
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
    char *owner;
    unsigned short *depthSlot;
    RenderMatrixSlot *matrixSlot;

    owner = func_800C2B50();
    stack.seed = s_EruptionShadowSeed;
    RotMatrix(&stack.seed.vector, &stack.generatedMatrix);
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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
    stack.matrix.t[1] = D_800942EC;
    stack.matrix.t[2] = fx->z;
    depthSlot = (unsigned short *)&g_RoomEruptionShadowPacket.depth;
    *depthSlot = fx->depth;
    matrixSlot = &D_800BCFA4;
    gte_ldrotmatrix(matrixSlot->value);
    gte_ldtransmatrix(matrixSlot->value);
    func_800C42A4((char *)depthSlot - 0xA, &stack.matrix, 1);
}

void RoomEffect_GroundEruptionNop2(void) {
}

/* Flings the six debris particles out at random angles. */
void RoomEffect_GroundEruptionInitDebris(void *arg0, void *arg1, char *state) {
    char *owner;
    unsigned int i;
    short *flag;
    int *life;
    char *particle;
    int angle;
    int wave;

    owner = func_800C2B50();
    func_800C2B28(0);
    angle = rand() % 0x1000;
    i = 0;
    flag = (short *)state;
    life = (int *)state;
    particle = state;
    do {
        PE1_COMPILER_LAUNDER(particle);
        *(short *)(particle + 0x0) = *(int *)(owner + 0x18);
        *(short *)(particle + 0x2) = *(int *)(owner + 0x1C);
        *(short *)(particle + 0x4) = *(int *)(owner + 0x20);

        wave = rsin(angle);
        *(short *)(particle + 0x30) =
            (wave * (rand() % 0x800 + 0x800)) >> 12;
        *(short *)(particle + 0x32) = -(rand() % 6096 + 3000);
        wave = rcos(angle);
        *(short *)(particle + 0x34) =
            (wave * (rand() % 0x800 + 0x800)) >> 12;

        i++;
        life[0x18] = 500;
        flag[0x3C] = 1;
        flag++;
        life++;
        particle += 8;
    } while (i < 6);

    *(short *)(state + 0x86) = 0x80;
    *(short *)(state + 0x84) = 0x400;
    *(short *)(state + 0x88) = 0x320;
}

/* Draws the live debris; the first particle also leaves a floor shadow. */
void RoomEffect_GroundEruptionDrawDebris(void *arg0, void *arg1, char *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    char *owner;
    int depth;
    register unsigned int i asm("$16");
    char *particle;
    u8 *packetFields;
    RoomFxSpritePacket *packet;
    u8 code;
    u8 mode;

    owner = func_800C2B50();
    PE1_COMPILER_USE(owner);
    depth = 0x80;
    i = 0;
    code = 0x42;
    mode = 3;
    packetFields = &g_RoomEruptionDebrisPacket.code;
    packet = (RoomFxSpritePacket *)(packetFields - 4);
    func_800C2EAC(owner[0x24]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
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

    particle = state;
    do {
        PE1_COMPILER_LAUNDER(particle);
        depth -= 20;
        if (*(short *)(state + 0x78 + i * 2) == 1) {
            matrix.t[0] = *(short *)(particle + 0);
            matrix.t[1] = *(short *)(particle + 2);
            matrix.t[2] = *(short *)(particle + 4);
            if (i == 0) {
                func_800C3238(2);
                packetFields[0] = code;
                packetFields[1] = mode;
                *(s16 *)(packetFields + 6) = 0xFF;
            } else {
                func_800C3238(3);
                packetFields[0] = 0x47;
                packetFields[1] = 5;
                *(s16 *)(packetFields + 6) = depth;
            }
            func_800C42A4(packet, &matrix, 1);
            if (i == 0) {
                matrix.t[0] = *(short *)(state + 0);
                matrix.t[1] = D_800942EC;
                matrix.t[2] = *(short *)(state + 4);
                g_RoomEruptionDebrisDepth = 0x40;
                g_RoomEruptionDebrisPacket.code = code;
                g_RoomEruptionDebrisPacket.mode = mode;
                func_800C3238(2);
                func_800C42A4(packet, &matrix, 1);
            }
        }
        i++;
        particle += 8;
    } while (i < 6);
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

typedef struct RoomEruptionTrailCursor {
    u8 pad00[0x76];
    u16 current;
    u16 next;
} RoomEruptionTrailCursor;

typedef union RoomEruptionDebris {
    struct {
        u8 x[2];
        u8 y[2];
        u8 z[2];
        u8 pad06[0x2A];
        u8 deltaX[2];
        u8 deltaY[2];
        u8 deltaZ[2];
        u8 pad36[0x2A];
        u8 deltaYStep[4];
        u8 pad64[0x14];
        u8 trailValues[12];
    } storage;
    RoomEruptionTrailEntry history[6];
    struct {
        u8 pad00[0x0A];
        RoomEruptionTrailCursor cursor;
    } trailView;
} RoomEruptionDebris;

/* Moves the lead debris particle under gravity and shifts its trail. */
void RoomEffect_GroundEruptionUpdateDebris(
    void *unused, RoomEruptionTrailTrigger *trigger, RoomEruptionDebris *object) {
    RoomEruptionTrailEntry *history;
    RoomEruptionTrailCursor *trail;
    int count;

    count = 5;
    trail = &object->trailView.cursor;
    history = &object->history[5];
    /* A do-while strength-reduces the cursor and moves it out of $t0. */
shiftTrail:
    *history = history[-1];
    history--;
    trail->next = trail->current;
    count--;
    trail = (RoomEruptionTrailCursor *)((u8 *)trail - 2);
    if (count != 0) {
        goto shiftTrail;
    }

    *(u16 *)object->storage.x += (s16)*(u16 *)object->storage.deltaX >> 8;
    *(u16 *)object->storage.y += (s16)*(u16 *)object->storage.deltaY >> 8;
    *(u16 *)object->storage.z += (s16)*(u16 *)object->storage.deltaZ >> 8;
    *(u16 *)object->storage.deltaY += *(s32 *)object->storage.deltaYStep;

    if (*(s16 *)object->storage.y > D_800942EC) {
        *(u16 *)object->storage.trailValues = 0;
    }
    if (trigger->timer == 60) {
        trigger->state = 2;
    }
}
