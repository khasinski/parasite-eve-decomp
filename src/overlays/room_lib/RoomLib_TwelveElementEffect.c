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
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "room_lib.h"
#include "pe1/room_floor.h"

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
void RoomLib_InitTwelveEffectPackets(char *obj, void *unused, char *state) {
    char *owner;
    RoomLibFxMatrixWords *words;

    func_800C2B40(state);
    *(void **)(state + 0x24) = func_8006DC18(0xB);
    owner = *(char **)(obj + 8);
    *(char **)state = owner;
    words = *(RoomLibFxMatrixWords **)(owner + 0x238);
    *(RoomLibFxMatrixWords *)(state + 4) = *words;
    *(short *)(state + 0x28) = 0x28;
    *(short *)(state + 0x2A) = 0;
    *(short *)(state + 0x2C) = 0;
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
    short *state = (short *)arg2;

    if (state[0x15] != 0) {
        state[0x15]--;
    }

    if (state[0x16] == 1) {
        int timer = state[0x15];

        state[0x16] = 0;
        if (timer == 0) {
            state[0x15] = *(unsigned short *)(arg2 + 0x28);
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}

typedef struct RoomTwelveEffectStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix sourceMatrix;
    RoomSpriteMatrix localMatrix;
} RoomTwelveEffectStack;

int *func_800C2B28(int index);
int func_800C66C8(void *owner, int id, RoomSpriteMatrix *matrix);


/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_InitTwelveElementEffect(void *owner, void *unused, char *state) {
    RoomTwelveEffectStack stack;
    void *effectOwner;
    register char *workState asm("$17");
    int spacing;
    RoomSpriteMatrix *localMatrix;
    register char *vector asm("$3");
    s16 *values;
    unsigned int i;
    int depth;
    int initialValue;
    char *clock;
    register RoomSpriteMatrix *sourceMatrix asm("$3");

    asm("" : "=r"(effectOwner), "=r"(workState) : "0"(owner), "1"(state));
    clock = func_800C2B50();
    stack.seed = s_TwelveEffectInitSeed;
    stack.sourceMatrix = *(RoomSpriteMatrix *)(clock + 4);

    *(s16 *)(workState + 0x26) = 0xC;
    *(s16 *)(workState + 0x20) = *func_800C2B28(0);
    *(s16 *)(workState + 0x22) = *func_800C2B28(1);
    *(s16 *)(workState + 0x24) = *func_800C2B28(2);
    spacing = *func_800C2B28(3);
    *(s16 *)&stack.seed = *func_800C2B28(4);
    *(s16 *)(workState + 0x2A) = *func_800C2B28(5);
    *(s16 *)(workState + 0x28) = *func_800C2B28(6);

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

    *(RoomSpriteMatrix *)workState = stack.sourceMatrix;
    i = 0;
    initialValue = 0x64;
    depth = 0;
    vector = workState;
    values = (s16 *)workState;
    do {
        *(workState + i + 0x2C) = 0;
        *(workState + i + 0x38) = 0;
        *(s16 *)((char *)values + 0x44) = initialValue;
        *(s16 *)(vector + 0x60) = depth;
        depth += spacing;
        *(s16 *)(vector + 0x5C) = 0;
        *(s16 *)(vector + 0x5E) = 0;
        vector += 8;
        values++;
        i++;
    } while (i < 12);
    *(int *)(workState + 0x11C) =
        func_800C66C8(effectOwner, 0x58F, &stack.sourceMatrix);
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
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
int func_8001CAB0(int x, int z, int arg2, int arg3);
int func_800C61A8(s16 *point, RoomSpriteMatrix *matrix);

extern int D_8009D248;
extern u16 D_8009D1CC;
extern char *D_8009D254;

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_DrawTwelveElementEffect(void *unused0, void *unused1,
                                     char *state) {
    RoomTwelveEffectDrawStack stack;
    char *workState;
    char *clock;
    unsigned int i;
    register RoomSpriteMatrix *primaryMatrix asm("$23");
    RoomFxVec4 *sourceScale;
    RoomSpriteMatrix *secondaryMatrix;
    register char *valueCursor asm("$19");
    char *element;
    register unsigned char *secondPacket asm("$16");
    unsigned int positionOffset;
    register int pointValue asm("$2");
    RoomSpriteMatrix *transformArg;

    asm("" : "=r"(workState) : "0"(state));
    clock = func_800C2B50();
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
    asm("" : : "r"(primaryMatrix), "r"(sourceScale));
    secondaryMatrix = &stack.secondaryMatrix;
    valueCursor = workState;
    func_800C2EAC(*(u8 *)(clock + 0x24));
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);

    do {
        element = workState + i;
        if (*(s8 *)(element + 0x2C) == 1) {
            stack.primaryMatrix = *(RoomSpriteMatrix *)workState;
            transformArg = primaryMatrix;
            asm("" : "=r"(transformArg) : "0"(transformArg));
            positionOffset = i << 3;
            positionOffset += 0x5C;
            ApplyMatrix(transformArg, workState + positionOffset,
                          stack.primaryMatrix.t);

            func_80071A44(sourceScale, 0, 0x10);
            stack.sourceScale.x = *(s16 *)(valueCursor + 0x44);
            stack.sourceScale.y = *(s16 *)(valueCursor + 0x44);
            stack.sourceScale.z = *(s16 *)(valueCursor + 0x44);
            stack.scale = stack.sourceScale;
            ScaleMatrix((GteMatrix *)primaryMatrix, (GteVector *)&stack.scale);

            stack.primaryMatrix.t[0] += *(int *)(clock + 0x18);
            stack.primaryMatrix.t[1] += *(int *)(clock + 0x1C);
            stack.primaryMatrix.t[2] += *(int *)(clock + 0x20);
            func_800C3134(RoomLib_TwelveEffectTable,
                          *(s8 *)(element + 0x38),
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

            func_800C42A4((unsigned char *)&RoomLib_TwelveEffectPrimaryPacket,
                          primaryMatrix, 0);
            stack.secondaryMatrix.t[1] = g_RoomFloorY->y;
            secondPacket = (unsigned char *)&RoomLib_TwelveEffectSecondaryPacket;
            func_800C3134(RoomLib_TwelveEffectTable,
                          *(s8 *)(element + 0x38), secondPacket);
            func_800C42A4(secondPacket, secondaryMatrix, 0);

            stack.collision[0] = stack.secondaryMatrix.t[0];
            stack.collision[1] = 0;
            stack.collision[2] = stack.secondaryMatrix.t[2];
            if (func_8001CAB0(stack.secondaryMatrix.t[0] << 16,
                              stack.secondaryMatrix.t[2] << 16,
                              D_8009D248, D_8009D1CC) == 0) {
                *(s8 *)(element + 0x2C) = -1;
            }

            pointValue = *(s16 *)(D_8009D254 + 0x2A);
            stack.point[0] = pointValue;
            pointValue = *(s16 *)(D_8009D254 + 0x2E);
            stack.point[1] = pointValue;
            pointValue = *(s16 *)(D_8009D254 + 0x32);
            stack.point[2] = pointValue;
            if (func_800C61A8(stack.point, secondaryMatrix) != 0) {
                *(s16 *)(clock + 0x2C) = 1;
            }
        }
        i++;
        valueCursor += 2;
    } while (i < 12);
}

ROOMLIB_TICK_12_COUNTERS(RoomLib_TickTwelveElementCounters)
