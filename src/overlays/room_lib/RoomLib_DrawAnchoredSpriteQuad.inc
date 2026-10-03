#include "pe1/field_script_context.h"
#include "RoomLib_RenderLayouts.h"
#ifndef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_FUNC
#error "ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_DEPTH
#error "ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_DEPTH must name the packet depth word"
#endif
#ifndef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_PACKET
#error "ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_PACKET must name the floor packet"
#endif

void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C2EAC(u8 owner);
void func_800C2FF0(s32 width, s32 height);
void func_800C3098(s32 depth);
void func_800C3238(int page);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, s32 mode);

extern RoomFxSeed8 D_8018EFF4;
extern short D_800942EC;
extern s16 ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_DEPTH;
extern u8 ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_PACKET[];

typedef struct RoomLibSpriteQuadStack {
    RoomSpriteMatrix scaleMatrix;
    RoomFxSeed8 seed;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomLibSpriteQuadStack;

/* Builds a uniformly scaled rotation from the shared seed and draws the
 * sprite twice at each of two map anchors: once unrotated at the anchor
 * height and once rotated on the shared floor height. Retail marks this
 * function handwritten; only its COP2 windows remain ASM. Like
 * RoomLib_DrawAnchoredSpritePair.inc, the four pointers are pinned to
 * s0..s3 and the seed argument is pinned behind one empty barrier because
 * the GTE operands give the matrix pointer the most references, which
 * otherwise hands it s0 and hoists its address above the seed load. */
void ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_FUNC(void *unused0, void *unused1,
                                            char *params) {
    RoomLibSpriteQuadStack stack;
    /* Retail keeps these in s0..s3; fx is reused for the packet pointers. */
    register char *fx asm("$16");
    char *map;
    RoomSpriteMatrix *scaleMatrix;
    register RoomSpriteMatrix *matrix asm("$19");
    char *root;
    short *floorY;
    int sprite;

    fx = params;
    root = func_800C2B50();
    stack.seed = D_8018EFF4;
    map = *(char **)(*(char **)root + 0x238);
    func_800C2EAC(*(u8 *)(root + 0x6C));
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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
    func_80078CC4(&stack.scaleMatrix, &stack.scale);
    {
        register RoomFxSeed8 *seedArg asm("$4") = &stack.seed;
        asm("" : : "r"(seedArg));
        matrix = &stack.matrix;
        func_800794C4(seedArg, matrix);
    }

    scaleMatrix = &stack.scaleMatrix;
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

    /* Retail reuses the params register for the packet pointers. */
    sprite = *(u16 *)(fx + 0x12);
    fx = (char *)&ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_DEPTH;
    *(s16 *)fx = sprite;
    stack.scaleMatrix.t[0] = *(int *)(map + 0x134);
    stack.scaleMatrix.t[1] = *(int *)(map + 0x138);
    fx -= 10;
    stack.scaleMatrix.t[2] = *(int *)(map + 0x13C);
    func_800C42A4(fx, scaleMatrix, 1);
    stack.scaleMatrix.t[0] = *(int *)(map + 0x1B4);
    stack.scaleMatrix.t[1] = *(int *)(map + 0x1B8);
    stack.scaleMatrix.t[2] = *(int *)(map + 0x1BC);
    func_800C42A4(fx, scaleMatrix, 1);

    fx = (char *)ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_PACKET;
    floorY = &D_800942EC;
    stack.matrix.t[0] = *(int *)(map + 0x134);
    stack.matrix.t[1] = *floorY;
    stack.matrix.t[2] = *(int *)(map + 0x13C);
    func_800C42A4(fx, matrix, 1);
    stack.matrix.t[0] = *(int *)(map + 0x1B4);
    stack.matrix.t[1] = *floorY;
    stack.matrix.t[2] = *(int *)(map + 0x1BC);
    func_800C42A4(fx, matrix, 1);
}

#undef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_FUNC
#undef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_DEPTH
#undef ROOMLIB_DRAW_ANCHORED_SPRITE_QUAD_PACKET
