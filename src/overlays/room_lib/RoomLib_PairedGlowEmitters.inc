/*
 * The paired glow, first piece: the class no-ops, the glow sprite's init,
 * the timer, the two emitters' init and update, and the glow sprite's draw
 * (src/overlays/room_lib/RoomLib_PairedGlow.c).
 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/room_effect_state.h"
#include "pe1/field_script_context.h"
#include "room_lib.h"
#include "RoomLib_RenderLayouts.h"
#include "pe1/room_paired_glow.h"

/* Copied into the emitter transforms' stack frames (both pieces). */
const RoomFxSeed8 RoomLib_PairedGlowSeed = {{0x00, 0x04, 0, 0, 0, 0, 0, 0}};

int RoomLib_PairedGlowNop6(void) {
    return 0;
}

int RoomLib_PairedGlowNop0(void) {
    return 0;
}

/* Init of the glow sprite: binds the state to the owner's matrix words,
 * fills the room's sprite record and loads its packet. */
void *RoomLib_InitPairedGlowSprite(char *obj, s32 arg1, char *state) {
    char *owner;
    RoomEffectWords8 *src;

    func_800C2B40(state);
    owner = *(char **)(obj + 0x8);
    *(char **)(state + 0x0) = owner;
    src = *(RoomEffectWords8 **)(owner + 0x238);
    *(RoomEffectWords8 *)(state + 0x4) = *src;
    *(s16 *)(state + 0x2A) = 0;
    *(s16 *)(state + 0x2C) = 0;
    *(s16 *)(state + 0x28) = 0x1E;
    *(void **)(state + 0x24) = func_8006DC18(0x23);

    g_RoomPairedGlowSprite.offset = -0x12C;
    g_RoomPairedGlowSprite.depth = 0x80;
    g_RoomPairedGlowSprite.code = 0;
    g_RoomPairedGlowSprite.mode = 0;
    g_RoomPairedGlowSprite.r = 0x80;
    g_RoomPairedGlowSprite.g = 0x80;
    g_RoomPairedGlowSprite.b = 0x80;
    g_RoomPairedGlowSprite.zero = 0;
    return g_RoomPairedGlowPacket = func_8006E498(D_800B0E64, 0xCB8704);
}

void RoomLib_PairedGlowNopA(void) {
}

/* Counts the state's timer down and re-arms it when the owner asks; ends
 * the object when the engine reports the effect done. */
void RoomLib_TickPairedGlowTimer(int arg0, char *arg1, char *arg2) {
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

void RoomLib_InitPairedEmitterWithSound(void *arg0, void *arg1,
                                      RoomFxPairedEmitterState *state) {
    RoomFxEmitterParams *primary;
    RoomFxEmitterParams *secondary;
    void *volatile *soundOwnerSlot;

    state->phase = 0;
    state->timer = 0;
    state->intensity = 0x80;
    primary = &state->primary;
    primary->mode = 0x10;
    primary->offset = 0;
    primary->intensity = 0x80;
    primary->color1[0] = *func_800C2B10(1);
    primary->color1[1] = *func_800C2B10(2);
    primary->color1[2] = *func_800C2B10(3);
    primary->color0[0] = *func_800C2B10(4);
    primary->color0[1] = *func_800C2B10(5);
    primary->color0[2] = *func_800C2B10(6);
    primary->extent0 = 0x4B0;
    primary->extent1 = 0x258;
    primary->source = state->sourceData;
    func_800C4E50(primary);

    secondary = &state->secondary;
    secondary->mode = 0x10;
    secondary->offset = 0;
    secondary->intensity = 0x80;
    secondary->color1[0] = *func_800C2B10(1) >> 1;
    secondary->color1[1] = *func_800C2B10(2) >> 1;
    secondary->color1[2] = *func_800C2B10(3) >> 1;
    secondary->color0[0] = *func_800C2B10(4) >> 1;
    secondary->color0[1] = *func_800C2B10(5) >> 1;
    secondary->color0[2] = *func_800C2B10(6) >> 1;
    secondary->extent0 = 0x4B0;
    secondary->extent1 = 0x12C;
    secondary->source = state->sourceData;
    func_800C4E50(secondary);

    soundOwnerSlot = &D_800B0E64;
    if (*soundOwnerSlot != 0) {
        func_8006DF50(*soundOwnerSlot, 0x57E, 0, 0x80, 0x7F);
    }
}

typedef struct RoomPairedEmitterStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix base_matrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 transformed_scale;
    RoomFxVec4 scale;
} RoomPairedEmitterStack;

void RoomLib_UpdatePairedEmitter(void *arg0, char *arg1,
                                        RoomFxPairedEmitterState *state) {
    RoomPairedEmitterStack stack;
    RoomClock *owner;
    RoomLink *link;
    unsigned char *table;

    owner = func_800C2B50();
    stack.seed = RoomLib_PairedGlowSeed;
    stack.base_matrix.m[2][2] = 0x1000;
    stack.base_matrix.m[1][1] = 0x1000;
    stack.base_matrix.m[0][0] = 0x1000;
    stack.base_matrix.t[2] = 0;
    stack.base_matrix.t[1] = 0;
    stack.base_matrix.t[0] = 0;
    stack.base_matrix.m[2][1] = 0;
    stack.base_matrix.m[2][0] = 0;
    stack.base_matrix.m[1][2] = 0;
    stack.base_matrix.m[1][0] = 0;
    stack.base_matrix.m[0][2] = 0;
    stack.base_matrix.m[0][1] = 0;

    func_80071A44(&stack.scale, 0, 0x10);
    stack.scale.x = state->timer * 2;
    stack.scale.y = state->timer * 2;
    stack.scale.z = state->timer * 2;
    stack.transformed_scale = stack.scale;
    ScaleMatrix(&stack.base_matrix, &stack.transformed_scale);
    RotMatrix(&stack.seed, &stack.matrix);

    /* PSY-Q gte_CompMatrix shape: MulMatrix0 in place, then the translation
     * through RTV0TR into the long-vector slot. */
    gte_ldrotmatrix(&stack.base_matrix);
    gte_ldclmv(&stack.matrix);
    gte_rtir();
    gte_stclmv(&stack.matrix);
    gte_ldclmv(stack.matrix.m[0] + 1);
    gte_rtir();
    gte_stclmv(stack.matrix.m[0] + 1);
    gte_ldclmv(stack.matrix.m[0] + 2);
    gte_rtir();
    gte_stclmv(stack.matrix.m[0] + 2);
    gte_ldtransmatrix(&stack.base_matrix);
    gte_ldlv0(stack.matrix.t);
    gte_rtv0tr_mac();
    gte_stlvl(stack.matrix.t);

    func_800C3238(2);

    table = g_RoomPairedGlowColorTable;
    link = (*(RoomLink **)owner)->p238;
    func_800C3134(table, *(s16 *)(arg1 + 2), &state->primary.color0);
    func_800C3134(table, *(s16 *)(arg1 + 2), &state->secondary.color0);
    state->primary.intensity = state->intensity;
    stack.matrix.t[0] = RW32(link, 0x14);
    stack.matrix.t[1] = RW32(link, 0x18) - 0x64;
    stack.matrix.t[2] = RW32(link, 0x1C);
    func_800C4FC4(&state->primary, &stack.matrix, 0);

    state->secondary.intensity = state->intensity;
    stack.matrix.t[0] = RW32(link, 0x14);
    stack.matrix.t[1] =
        ((RoomFxFrameCounterRecord *)&D_800942EC)->count;
    stack.matrix.t[2] = RW32(link, 0x1C);
    func_800C4FC4(&state->secondary, &stack.matrix, 0);
}

ROOMLIB_PARTICLE_TICK_A(RoomLib_TickPairedGlowA)

void RoomLib_PairedGlowNopB(void) {
}

void RoomLib_PairedGlowNopC(void) {
}

void RoomLib_PairedGlowNopD(void) {
}

ROOMLIB_SET3_RESET(RoomLib_ResetPairedGlowScale, 0x100, 0x1)

typedef struct RoomScaledSpriteStack {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomScaledSpriteStack;

void RoomLib_DrawPairedGlowSprite(char *object, void *unused, char *state) {
    RoomScaledSpriteStack stack;
    char *entity;
    char *workState;
    char *clock;
    register int callArg0 asm("$4");
    register int callArg1 asm("$5");
    int handleA;
    int handleB;

    entity = object;
    workState = state;
    clock = func_800C2B50();
    RotMatrix(workState + 8, &stack.matrix);

    stack.matrix.t[0] = *(s16 *)(*(char **)(entity + 8) + 0x2A);
    stack.matrix.t[1] = *(s16 *)(*(char **)(entity + 8) + 0x2E);
    stack.matrix.t[2] = *(s16 *)(*(char **)(entity + 8) + 0x32);

    func_80071A44(&stack.sourceScale, 0, 0x10);
    stack.sourceScale.x = *(s16 *)(workState + 0x10);
    stack.sourceScale.y = 0xC8;
    stack.sourceScale.z = *(s16 *)(workState + 0x10);
    stack.scale = stack.sourceScale;
    ScaleMatrix(&stack.matrix, &stack.scale);

    func_800C6D5C(g_RoomPairedGlowPacket, 0, 0);
    if (*(int *)(clock + 0x24) == 0) {
        handleA = func_80077A64(0, 1, 0x340, 0x100);
        callArg0 = 0;
        callArg1 = 0x1D7;
        asm("" : : "r"(callArg0), "r"(callArg1));
        handleA = (u16)handleA;
        handleB = func_80077AA4(callArg0, callArg1);
        func_800C6EC0((u16)handleA, (u16)handleB);
    }
    if (*(int *)(clock + 0x24) == 1) {
        handleA = func_80077A64(0, 1, 0x340, 0x160);
        callArg0 = 0;
        callArg1 = 0x1DB;
        asm("" : : "r"(callArg0), "r"(callArg1));
        handleA = (u16)handleA;
        handleB = func_80077AA4(callArg0, callArg1);
        func_800C6EC0((u16)handleA, (u16)handleB);
    }
    func_800C6ED8(1);
    func_800C6EF8(g_RoomPairedGlowPacket);
    func_800C7098(g_RoomPairedGlowPacket, 0x60, 0x10, 0x80);
    func_800C6FA0(g_RoomPairedGlowPacket, *(u16 *)(workState + 0x12));
    func_800C71E4(g_RoomPairedGlowPacket, &stack.matrix);
    func_800C6F4C(g_RoomPairedGlowPacket);
}
