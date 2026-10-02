#include "pe1/field_script_context.h"
#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#ifndef ROOMLIB_DRAW_FOUR_SPRITES_FUNC
#error "ROOMLIB_DRAW_FOUR_SPRITES_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_DRAW_FOUR_SPRITES_SEED
#error "ROOMLIB_DRAW_FOUR_SPRITES_SEED must name the matrix seed"
#endif
#ifndef ROOMLIB_DRAW_FOUR_SPRITES_DEPTH
#error "ROOMLIB_DRAW_FOUR_SPRITES_DEPTH must name the packet depth field"
#endif

typedef struct RoomFourSpriteColor {
    u8 r, g, b, pad;
} RoomFourSpriteColor;

typedef struct RoomFourSpritePosition {
    s16 x, y, z, pad;
} RoomFourSpritePosition;

/* Per-room sprite set: the shared rotation matrix followed by the four
 * sprites' colors, positions, scales and packet depths. */
typedef struct RoomFourSpriteState {
    RoomSpriteMatrix rotation;            /* 0x00 */
    RoomFourSpriteColor color[4];         /* 0x20 */
    RoomFourSpritePosition position[4];   /* 0x30 */
    u8 reserved50[0x20];                  /* 0x50 */
    s16 scale[4];                         /* 0x70 */
    u16 depth[4];                         /* 0x78 */
} RoomFourSpriteState;

typedef struct RoomFourSpriteStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomFourSpriteStack;

void func_800C2EAC(u8 owner);
void func_800C2FF0(int width, int height);
void func_800C3098(int depth);
void func_800C3238(int mode);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void memset(void *dst, int value, int size);
void Gte_ScaleMatrix(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);

extern RoomFxSeed8 ROOMLIB_DRAW_FOUR_SPRITES_SEED;
extern unsigned short ROOMLIB_DRAW_FOUR_SPRITES_DEPTH;

/* Draws four scaled sprites. The state begins with the shared rotation
 * matrix; each sprite reads its position, scale, depth and color from the
 * arrays that follow it. Retail marks this function handwritten; only its
 * COP2 windows remain ASM. */
void ROOMLIB_DRAW_FOUR_SPRITES_FUNC(void *unused0, void *unused1,
                                    RoomFourSpriteState *state) {
    RoomFourSpriteStack stack;
    char *clock;
    unsigned int i;
    /* Retail keeps the matrix base in s3 and the second column in s7; the
     * three sprite-array cursors are strength-reduced copies the compiler
     * places in s0..s2 after them. */
    register RoomSpriteMatrix *matrix asm("$19");
    register short *column1 asm("$23");
    /* One scratch pointer serves the last column and the translation, so
     * the loop optimizer does not hoist either address. */
    unsigned short *work;
    unsigned short *depthSlot;

    clock = func_800C2B50();
    stack.seed = ROOMLIB_DRAW_FOUR_SPRITES_SEED;
    i = 0;
    matrix = &stack.matrix;
    column1 = &stack.matrix.m[0][1];
    depthSlot = &ROOMLIB_DRAW_FOUR_SPRITES_DEPTH;
    func_800C2EAC(clock[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x100);
    func_800C3238(2);

    for (i = 0; i < 4; i++) {
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
        stack.matrix.t[0] = state->position[i].x;
        stack.matrix.t[1] = state->position[i].y;
        stack.matrix.t[2] = state->position[i].z;

        gte_ldrotmatrix(&state->rotation);
        gte_ldrtir12_matrix_column(matrix);
        gte_stir123_column(matrix);
        gte_ldrtir12_matrix_column(column1);
        gte_stir123_column(column1);
        work = (unsigned short *)&stack.matrix.m[0][2];
        gte_ldrtir12_matrix_column(work);
        gte_stir123_column(work);
        gte_ldtransmatrix(&state->rotation);
        work = (unsigned short *)stack.matrix.t;
        gte_ldv0_word3(work);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_mvmva_rotation_v0_translation_sf12();
        gte_swc2_9_0(work);
        gte_swc2_10_4(work);
        gte_swc2_11_8(work);

        func_800794C4(&stack.seed, matrix);
        memset(&stack.sourceScale, 0, 0x10);
        stack.sourceScale.x = state->scale[i];
        stack.sourceScale.y = state->scale[i];
        stack.sourceScale.z = state->scale[i];
        stack.scale = stack.sourceScale;
        Gte_ScaleMatrix(matrix, &stack.scale);
        *depthSlot = state->depth[i];
        ((u8 *)depthSlot)[-0xA] = state->color[i].r;
        ((u8 *)depthSlot)[-0x9] = state->color[i].g;
        ((u8 *)depthSlot)[-0x8] = state->color[i].b;
        func_800C42A4((char *)depthSlot - 0xA, matrix, 1);
    }
}

#undef ROOMLIB_DRAW_FOUR_SPRITES_FUNC
#undef ROOMLIB_DRAW_FOUR_SPRITES_SEED
#undef ROOMLIB_DRAW_FOUR_SPRITES_DEPTH
