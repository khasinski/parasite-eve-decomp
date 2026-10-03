#include "pe1/field_script_context.h"
#include "room_lib.h"

#ifndef ROOMLIB_UPDATE_PAIRED_EMITTER_NAME
#define ROOMLIB_UPDATE_PAIRED_EMITTER_NAME func_8018F6D8
#endif

/* Overlay-local label of the frame counter at 0x800942EC. */
#ifndef ROOMLIB_UPDATE_PAIRED_EMITTER_COUNTER
#define ROOMLIB_UPDATE_PAIRED_EMITTER_COUNTER D_800942EC
#endif

#ifndef ROOMLIB_UPDATE_PAIRED_EMITTER_TABLE
#define ROOMLIB_UPDATE_PAIRED_EMITTER_TABLE D_80193660
#endif

typedef struct Room118Stack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix base_matrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 transformed_scale;
    RoomFxVec4 scale;
} Room118Stack;

extern RoomFxSeed8 D_8018EFF4;
extern unsigned char ROOMLIB_UPDATE_PAIRED_EMITTER_TABLE[];

void func_80071A44(RoomFxVec4 *arg0, s32 arg1, s32 arg2);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C3134(void *arg0, s32 arg1, void *arg2);
void func_800C4FC4(RoomFxEmitterParams *params, RoomSpriteMatrix *matrix,
                   s32 arg2);

void ROOMLIB_UPDATE_PAIRED_EMITTER_NAME(void *arg0, char *arg1,
                                        RoomFxPairedEmitterState *state) {
    Room118Stack stack;
    RoomClock *owner;
    RoomLink *link;
    unsigned char *table;

    owner = func_800C2B50();
    stack.seed = D_8018EFF4;
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
    func_80078CC4(&stack.base_matrix, &stack.transformed_scale);
    func_800794C4(&stack.seed, &stack.matrix);

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

    table = ROOMLIB_UPDATE_PAIRED_EMITTER_TABLE;
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
        ((RoomFxFrameCounterRecord *)&ROOMLIB_UPDATE_PAIRED_EMITTER_COUNTER)->count;
    stack.matrix.t[2] = RW32(link, 0x1C);
    func_800C4FC4(&state->secondary, &stack.matrix, 0);
}

#undef ROOMLIB_UPDATE_PAIRED_EMITTER_NAME
#undef ROOMLIB_UPDATE_PAIRED_EMITTER_TABLE
#undef ROOMLIB_UPDATE_PAIRED_EMITTER_COUNTER
