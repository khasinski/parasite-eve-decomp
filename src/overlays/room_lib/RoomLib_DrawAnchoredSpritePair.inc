#include "pe1/field_script_context.h"
#include "RoomLib_RenderLayouts.h"
#ifndef ROOMLIB_DRAW_ANCHORED_SPRITE_PAIR_FUNC
#error "ROOMLIB_DRAW_ANCHORED_SPRITE_PAIR_FUNC must name the room entry point"
#endif

void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C3238(int page);
void func_800C4FC4(void *state, RoomSpriteMatrix *matrix, int mode);

extern RoomFxSeed8 D_8018EFF4;
extern short D_800942EC;

/* Builds a uniformly scaled rotation from the shared seed and draws the
 * sprite at two map anchors. Retail marks this function handwritten; only
 * its COP2 windows remain ASM. */
void ROOMLIB_DRAW_ANCHORED_SPRITE_PAIR_FUNC(void *unused0, void *unused1,
                                            char *params) {
    RoomLibSpriteTransformStack stack;
    /* Retail keeps these in s0..s3; the later locals reuse the same slots. */
    register RoomSpriteMatrix *scaleMatrix asm("$16");
    register RoomSpriteMatrix *matrix asm("$17");
    register char *fx asm("$18");
    register char *root asm("$19");
    register char *map asm("$16");
    register short *floorY asm("$18");
    register char *renderState asm("$19");
    int firstY;
    int firstX;
    int secondX;
    int secondY;

    fx = params;
    root = func_800C2B50();
    stack.seed = D_8018EFF4;

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

    stack.sourceScale.x = *(s16 *)(fx + 0x10);
    stack.sourceScale.y = *(s16 *)(fx + 0x10);
    stack.sourceScale.z = *(s16 *)(fx + 0x10);
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    func_80078CC4(scaleMatrix, &stack.scale);
    {
        register RoomFxSeed8 *seedArg asm("$4") = &stack.seed;
        asm("" : : "r"(seedArg));
        matrix = &stack.matrix;
        func_800794C4(seedArg, matrix);
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
        gte_mvmva_rotation_v0_translation_sf12();
        gte_swc2_9_0(translation);
        gte_swc2_10_4(translation);
        gte_swc2_11_8(translation);
    }

    func_800C3238(2);
    map = *(char **)(*(char **)root + 0x238);
    renderState = fx + 0x114;
    *(s16 *)(fx + 0x128) = *(u16 *)(fx + 0x12);
    /* Both anchors sit on the shared floor height; the first block stores
     * the height before the x coordinate, which fixes retail's register
     * choice. */
    floorY = &D_800942EC;
    firstX = *(int *)(map + 0x134);
    firstY = *floorY;
    stack.matrix.t[1] = firstY;
    stack.matrix.t[0] = firstX;
    stack.matrix.t[2] = *(int *)(map + 0x13C);
    func_800C4FC4(renderState, matrix, 1);
    secondX = *(int *)(map + 0x1B4);
    secondY = *floorY;
    stack.matrix.t[0] = secondX;
    stack.matrix.t[1] = secondY;
    stack.matrix.t[2] = *(int *)(map + 0x1BC);
    func_800C4FC4(renderState, matrix, 1);
}

#undef ROOMLIB_DRAW_ANCHORED_SPRITE_PAIR_FUNC
