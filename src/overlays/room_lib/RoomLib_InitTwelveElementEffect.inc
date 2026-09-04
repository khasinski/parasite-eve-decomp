#ifndef ROOMLIB_INIT_TWELVE_EFFECT_FUNC
#error "ROOMLIB_INIT_TWELVE_EFFECT_FUNC must name the room entry point"
#endif

typedef struct RoomTwelveEffectStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix sourceMatrix;
    RoomSpriteMatrix localMatrix;
} RoomTwelveEffectStack;

char *func_800C2B50(void);
int *func_800C2B28(int index);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
int func_800C66C8(void *owner, int id, RoomSpriteMatrix *matrix);

extern RoomFxSeed8 D_8018EFF4;

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void ROOMLIB_INIT_TWELVE_EFFECT_FUNC(void *owner, void *unused, char *state) {
    RoomTwelveEffectStack stack;
    register void *effectOwner asm("$19");
    register char *workState asm("$17");
    register int spacing asm("$18");
    register RoomSpriteMatrix *localMatrix asm("$16");
    register char *vector asm("$3");
    register s16 *values asm("$5");
    register unsigned int i asm("$4");
    register int depth asm("$6");
    register int initialValue asm("$7");
    char *clock;

    asm("" : "=r"(effectOwner), "=r"(workState) : "0"(owner), "1"(state));
    clock = func_800C2B50();
    stack.seed = D_8018EFF4;
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
    func_800794C4(&stack.seed, localMatrix);

    gte_ldrotmatrix(&stack.sourceMatrix);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][0]);
    gte_stir123_matrix_column(&stack.sourceMatrix.m[0][0]);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][1]);
    gte_stir123_matrix_column(&stack.sourceMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(&localMatrix->m[0][2]);
    gte_stir123_matrix_column(&stack.sourceMatrix.m[0][2]);
    gte_ldtransmatrix(&stack.sourceMatrix);
    gte_ldv0_short3(localMatrix->t);
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
    asm("" : "=r"(i), "=r"(initialValue), "=r"(depth), "=r"(vector),
        "=r"(values) : "0"(i), "1"(initialValue), "2"(depth),
        "3"(vector), "4"(values));
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

#undef ROOMLIB_INIT_TWELVE_EFFECT_FUNC
