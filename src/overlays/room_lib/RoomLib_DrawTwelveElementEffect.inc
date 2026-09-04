#ifndef ROOMLIB_DRAW_TWELVE_EFFECT_FUNC
#error "ROOMLIB_DRAW_TWELVE_EFFECT_FUNC must name the room entry point"
#endif

#ifndef ROOMLIB_TWELVE_EFFECT_TABLE
#error "ROOMLIB_TWELVE_EFFECT_TABLE must name the sprite table"
#endif

#ifndef ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET
#error "ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET must name the primary packet"
#endif

#ifndef ROOMLIB_TWELVE_EFFECT_SECONDARY_PACKET
#error "ROOMLIB_TWELVE_EFFECT_SECONDARY_PACKET must name the secondary packet"
#endif

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

char *func_800C2B50(void);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);
void func_800C2EAC(u8 owner);
void func_800C2FF0(int width, int height);
void func_800C3098(int depth);
void func_800C3238(int mode);
void func_80078BE4(RoomSpriteMatrix *matrix, void *position, int *translation);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C3134(void *table, int step, void *out);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
int func_8001CAB0(int x, int z, int arg2, int arg3);
int func_800C61A8(s16 *point, RoomSpriteMatrix *matrix);

extern RoomFxSeed8 D_8018EFFC;
extern RoomFxVec4 D_8018F004;
extern unsigned char ROOMLIB_TWELVE_EFFECT_TABLE[];
extern unsigned char ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET[];
extern unsigned char ROOMLIB_TWELVE_EFFECT_SECONDARY_PACKET[];
extern int D_8009D248;
extern u16 D_8009D1CC;
extern char *D_8009D254;
extern s16 D_800942EC;

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void ROOMLIB_DRAW_TWELVE_EFFECT_FUNC(void *unused0, void *unused1,
                                     char *state) {
    RoomTwelveEffectDrawStack stack;
    register char *workState asm("$22");
    register char *clock asm("$21");
    register unsigned int i asm("$20");
    register RoomSpriteMatrix *primaryMatrix asm("$23");
    RoomFxVec4 *sourceScale;
    register RoomSpriteMatrix *secondaryMatrix asm("$17");
    register char *valueCursor asm("$19");
    register char *element asm("$18");
    register unsigned char *secondPacket asm("$16");
    register unsigned int positionOffset asm("$5");
    register int pointValue asm("$2");
    register RoomSpriteMatrix *transformArg asm("$4");

    asm("" : "=r"(workState) : "0"(state));
    clock = func_800C2B50();
    stack.seed = D_8018EFFC;
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
    func_800794C4(&stack.seed, &stack.localMatrix);

    i = 0;
    primaryMatrix = &stack.primaryMatrix;
    sourceScale = &stack.sourceScale;
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
            asm("" : "=r"(positionOffset) : "0"(positionOffset));
            positionOffset += 0x5C;
            asm("" : "=r"(positionOffset) : "0"(positionOffset));
            func_80078BE4(transformArg, workState + positionOffset,
                          stack.primaryMatrix.t);

            func_80071A44(sourceScale, 0, 0x10);
            stack.sourceScale.x = *(s16 *)(valueCursor + 0x44);
            stack.sourceScale.y = *(s16 *)(valueCursor + 0x44);
            stack.sourceScale.z = *(s16 *)(valueCursor + 0x44);
            stack.scale = stack.sourceScale;
            func_80078CC4(primaryMatrix, &stack.scale);

            stack.primaryMatrix.t[0] += *(int *)(clock + 0x18);
            stack.primaryMatrix.t[1] += *(int *)(clock + 0x1C);
            stack.primaryMatrix.t[2] += *(int *)(clock + 0x20);
            func_800C3134(ROOMLIB_TWELVE_EFFECT_TABLE,
                          *(s8 *)(element + 0x38),
                          ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET);

            stack.secondaryMatrix = stack.primaryMatrix;
            stack.sourceScale = D_8018F004;
            func_80078CC4(secondaryMatrix, sourceScale);

            gte_ldrotmatrix(secondaryMatrix);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][0]);
            gte_stir123_matrix_column(&secondaryMatrix->m[0][0]);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][1]);
            gte_stir123_matrix_column(&stack.secondaryMatrix.m[0][1]);
            gte_ldrtir12_matrix_column(&stack.localMatrix.m[0][2]);
            gte_stir123_matrix_column(&stack.secondaryMatrix.m[0][2]);
            gte_ldtransmatrix(secondaryMatrix);
            gte_ldv0_short3(stack.localMatrix.t);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtv0tr_sf0();
            gte_swc2_9_0(stack.secondaryMatrix.t);
            gte_swc2_10_4(stack.secondaryMatrix.t);
            gte_swc2_11_8(stack.secondaryMatrix.t);

            func_800C42A4(ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET,
                          primaryMatrix, 0);
            stack.secondaryMatrix.t[1] = D_800942EC;
            secondPacket = ROOMLIB_TWELVE_EFFECT_SECONDARY_PACKET;
            func_800C3134(ROOMLIB_TWELVE_EFFECT_TABLE,
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

#undef ROOMLIB_DRAW_TWELVE_EFFECT_FUNC
#undef ROOMLIB_TWELVE_EFFECT_TABLE
#undef ROOMLIB_TWELVE_EFFECT_PRIMARY_PACKET
#undef ROOMLIB_TWELVE_EFFECT_SECONDARY_PACKET
