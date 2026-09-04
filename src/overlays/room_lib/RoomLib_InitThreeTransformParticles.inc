#ifndef ROOMLIB_INIT_THREE_TRANSFORM_PARTICLES_FUNC
#error "ROOMLIB_INIT_THREE_TRANSFORM_PARTICLES_FUNC must name the room entry point"
#endif

typedef struct RoomInitThreeVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomInitThreeVec;

typedef struct RoomInitThreeStack {
    RoomInitThreeVec projectionSeed;
    RoomInitThreeVec firstSeed;
    RoomInitThreeVec secondSeed;
    RoomSpriteMatrix firstMatrix;
    RoomSpriteMatrix secondMatrix;
} RoomInitThreeStack;

char *func_800C2B50(void);
int *func_800C2B28(int index);
int *func_800C2B10(int index);
void func_80079754(RoomInitThreeVec *seed, RoomSpriteMatrix *matrix);
void func_80078C34(RoomSpriteMatrix *matrix, RoomInitThreeVec *seed,
                   s16 *out);
int func_80071A54(void);
void func_800C66C8(void *owner, int id, void *state);

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void ROOMLIB_INIT_THREE_TRANSFORM_PARTICLES_FUNC(void *owner, void *unused,
                                                  char *state) {
    RoomInitThreeStack stack;
    register void *callOwner asm("$23");
    register char *workState asm("$21");
    register char *clock asm("$22");
    register int i asm("$20");
    register char *byteCursor asm("$16");
    register char *positionCursor asm("$17");
    register char *velocityCursor asm("$18");
    register char *timerCursor asm("$19");
    register int parameterIndex asm("$4");
    char *clockResult;
    int *value;
    int randomValue;

    asm("" : "=r"(callOwner), "=r"(workState) : "0"(owner), "1"(state));
    clockResult = func_800C2B50();
    parameterIndex = 3;
    clock = clockResult;
    asm("" : "=r"(parameterIndex) : "0"(parameterIndex));
    asm("" : "=r"(clock) : "0"(clock));
    stack.firstSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.firstSeed.y = -*value;
    stack.firstSeed.z = 0;
    func_80079754(&stack.firstSeed, &stack.firstMatrix);

    parameterIndex = 3;
    asm("" : "=r"(parameterIndex) : "0"(parameterIndex));
    stack.secondSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.secondSeed.y = *value;
    stack.secondSeed.z = 0;
    func_80079754(&stack.secondSeed, &stack.secondMatrix);

    gte_ldrotmatrix(&stack.firstMatrix);
    gte_ldrtir12_matrix_column(clock + 4);
    gte_stir123_matrix_column(&stack.firstMatrix.m[0][0]);
    gte_ldrtir12_matrix_column(clock + 6);
    gte_stir123_matrix_column(&stack.firstMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(clock + 8);
    gte_stir123_matrix_column(&stack.firstMatrix.m[0][2]);
    gte_ldtransmatrix(&stack.firstMatrix);
    gte_ldv0_short3(clock + 0x18);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(stack.firstMatrix.t);
    gte_swc2_10_4(stack.firstMatrix.t);
    gte_swc2_11_8(stack.firstMatrix.t);

    gte_ldrotmatrix(&stack.secondMatrix);
    gte_ldrtir12_matrix_column(clock + 4);
    gte_stir123_matrix_column(&stack.secondMatrix.m[0][0]);
    gte_ldrtir12_matrix_column(clock + 6);
    gte_stir123_matrix_column(&stack.secondMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(clock + 8);
    gte_stir123_matrix_column(&stack.secondMatrix.m[0][2]);
    gte_ldtransmatrix(&stack.secondMatrix);
    gte_ldv0_short3(clock + 0x18);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(stack.secondMatrix.t);
    gte_swc2_10_4(stack.secondMatrix.t);
    gte_swc2_11_8(stack.secondMatrix.t);

    i = 0;
    velocityCursor = workState;
    positionCursor = workState;
    timerCursor = workState;
    do {
        byteCursor = workState + i;
        randomValue = func_80071A54();
        *(u8 *)(byteCursor + 0x1E) = 0;
        *(u8 *)(byteCursor + 0x1A) = (randomValue % 10) + 1;
        *(s16 *)(timerCursor + 0x22) = *func_800C2B28(2);
        *(s32 *)(positionCursor + 0x3C) = *(s32 *)(clock + 0x18) << 16;
        *(s32 *)(positionCursor + 0x40) = *(s32 *)(clock + 0x1C) << 16;
        *(s32 *)(positionCursor + 0x44) = *(s32 *)(clock + 0x20) << 16;
        *(s16 *)(velocityCursor + 0x6C) = 0;
        *(s16 *)(velocityCursor + 0x6E) = 0;
        *(s16 *)(velocityCursor + 0x70) = 0;
        *(s16 *)(timerCursor + 0x2A) = 0;
        *(u8 *)(byteCursor + 0x0E) = 1;
        *(u8 *)(byteCursor + 0x16) = 0;
        *(u8 *)(byteCursor + 0x12) = 0;
        i++;
        velocityCursor += 8;
        positionCursor += 0x10;
        timerCursor += 2;
    } while (i < 3);

    *(s16 *)(workState + 0x00) = 3;
    *(s16 *)(workState + 0x02) = *func_800C2B28(0);
    *(s16 *)(workState + 0x04) = *func_800C2B28(1);
    *(s16 *)(workState + 0x06) = *func_800C2B28(5);
    *(u8 *)(workState + 0x0A) = *func_800C2B28(4);
    *(s16 *)(workState + 0x08) = *func_800C2B10(1);

    stack.projectionSeed.x = 0;
    stack.projectionSeed.y = 0;
    stack.projectionSeed.z = 0x1000;
    func_80078C34(&stack.firstMatrix, &stack.projectionSeed,
                  (s16 *)(workState + 0x9C));
    byteCursor = clock + 4;
    func_80078C34((RoomSpriteMatrix *)byteCursor, &stack.projectionSeed,
                  (s16 *)(workState + 0xA4));
    func_80078C34(&stack.secondMatrix, &stack.projectionSeed,
                  (s16 *)(workState + 0xAC));
    func_800C66C8(callOwner, 0x573, byteCursor);
}

#undef ROOMLIB_INIT_THREE_TRANSFORM_PARTICLES_FUNC
