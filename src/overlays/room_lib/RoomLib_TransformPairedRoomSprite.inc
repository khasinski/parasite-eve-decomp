#ifndef ROOMLIB_TRANSFORM_PAIRED_SPRITE_FUNC
#error "ROOMLIB_TRANSFORM_PAIRED_SPRITE_FUNC must name the room entry point"
#endif

typedef struct RoomPairedTransformStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix scaleMatrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomPairedTransformStack;

char *func_800C2B50(void);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C3238(int page);
void func_800C3134(void *table, int step, void *out);
void func_800C4FC4(void *state, RoomSpriteMatrix *matrix, int mode);

extern RoomFxSeed8 D_8018EFF4;
extern unsigned char ROOMLIB_PAIRED_SPRITE_TABLE[];

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void ROOMLIB_TRANSFORM_PAIRED_SPRITE_FUNC(void *unused, char *params,
                                          char *state) {
    RoomPairedTransformStack stack;
    register RoomSpriteMatrix *scaleMatrix asm("$16");
    register unsigned char *table asm("$17");
    register char *workState asm("$18");
    register RoomSpriteMatrix *matrix asm("$19");
    register char *root asm("$20");
    register char *renderParams asm("$21");
    register char *owner asm("$16");

    renderParams = params;
    workState = state;
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

    stack.sourceScale.x = *(s16 *)(workState + 0x138) * 2;
    stack.sourceScale.y = *(s16 *)(workState + 0x138) * 2;
    stack.sourceScale.z = *(s16 *)(workState + 0x138) * 2;
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    func_80078CC4(scaleMatrix, &stack.scale);
    matrix = &stack.matrix;
    func_800794C4(&stack.seed, matrix);

    gte_ldrotmatrix(scaleMatrix);
    gte_ldrtir12_matrix_column(&matrix->m[0][0]);
    gte_stir123_matrix_column(&matrix->m[0][0]);
    gte_ldrtir12_matrix_column(&matrix->m[0][1]);
    gte_stir123_matrix_column(&matrix->m[0][1]);
    gte_ldrtir12_matrix_column(&matrix->m[0][2]);
    gte_stir123_matrix_column(&matrix->m[0][2]);
    gte_ldtransmatrix(scaleMatrix);
    gte_ldv0_short3(matrix->t);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(matrix->t);
    gte_swc2_10_4(matrix->t);
    gte_swc2_11_8(matrix->t);

    func_800C3238(2);
    table = ROOMLIB_PAIRED_SPRITE_TABLE;
    owner = *(char **)(*(char **)root + 0x238);
    func_800C3134(table, *(s16 *)(renderParams + 2), workState + 0x10C);
    func_800C3134(table, *(s16 *)(renderParams + 2), workState + 0x124);
    *(s16 *)(workState + 0x11C) = *(u16 *)(workState + 0x13A);
    matrix->t[0] = *(int *)(owner + 0x14);
    matrix->t[1] = *(int *)(owner + 0x18) - 0x64;
    matrix->t[2] = *(int *)(owner + 0x1C);
    func_800C4FC4(workState + 0x108, matrix, 0);
}

#undef ROOMLIB_TRANSFORM_PAIRED_SPRITE_FUNC
#undef ROOMLIB_PAIRED_SPRITE_TABLE
