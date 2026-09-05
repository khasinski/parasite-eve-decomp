#ifndef ROOMLIB_TRANSFORM_SIXTEEN_POINT_EFFECT_FUNC
#error "ROOMLIB_TRANSFORM_SIXTEEN_POINT_EFFECT_FUNC must name the room entry point"
#endif

typedef struct RoomSixteenTransformStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix scaleMatrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomSixteenTransformStack;

char *func_800C2B50(void);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void Gte_ScaleMatrix(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C3238(int page);
void func_800C4FC4(void *state, RoomSpriteMatrix *matrix, int mode);
extern RoomFxSeed8 D_8018EFF4;

/* The retail function is marked handwritten and uses four explicit GTE windows. */
void ROOMLIB_TRANSFORM_SIXTEEN_POINT_EFFECT_FUNC(void *arg0, void *arg1,
                                                  char *state) {
    RoomSixteenTransformStack stack;
    register RoomSpriteMatrix *matrix asm("$16");
    register RoomSpriteMatrix *scaleMatrix asm("$17");
    register char *workState asm("$18");
    register char *owner asm("$19");

    asm("" : "=r"(workState) : "0"(state));
    owner = func_800C2B50();
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

    stack.sourceScale.x = *(s16 *)(workState + 0x120);
    stack.sourceScale.y = *(s16 *)(workState + 0x120);
    stack.sourceScale.z = *(s16 *)(workState + 0x120);
    stack.scale = stack.sourceScale;
    scaleMatrix = &stack.scaleMatrix;
    Gte_ScaleMatrix(scaleMatrix, &stack.scale);
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
    {
        unsigned short *translation = (unsigned short *)matrix->t;
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
    matrix->t[0] = *(int *)(owner + 0x18);
    matrix->t[1] = *(int *)(owner + 0x1C);
    matrix->t[2] = *(int *)(owner + 0x20);
    func_800C4FC4(workState + 0x108, matrix, 0);
}

#undef ROOMLIB_TRANSFORM_SIXTEEN_POINT_EFFECT_FUNC
