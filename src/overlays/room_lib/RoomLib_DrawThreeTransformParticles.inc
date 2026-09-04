#ifndef ROOMLIB_DRAW_THREE_PARTICLES_FUNC
#error "ROOMLIB_DRAW_THREE_PARTICLES_FUNC must name the room entry point"
#endif

#ifndef ROOMLIB_DRAW_THREE_PARTICLES_TABLE
#error "ROOMLIB_DRAW_THREE_PARTICLES_TABLE must name the sprite table"
#endif

#ifndef ROOMLIB_DRAW_THREE_PARTICLES_PACKET
#error "ROOMLIB_DRAW_THREE_PARTICLES_PACKET must name the primary packet"
#endif

#ifndef ROOMLIB_DRAW_THREE_PARTICLES_PRIMARY_ALPHA
#error "ROOMLIB_DRAW_THREE_PARTICLES_PRIMARY_ALPHA must name the primary alpha field"
#endif

#ifndef ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_ALPHA
#error "ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_ALPHA must name the floor alpha field"
#endif

#ifndef ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_HEIGHT
#error "ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_HEIGHT must name the floor height"
#endif

typedef struct RoomDrawThreeVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} RoomDrawThreeVec;

typedef struct RoomDrawThreeStack {
    RoomSpriteMatrix primaryMatrix;
    RoomSpriteMatrix secondaryMatrix;
    RoomFxSeed8 seed;
    RoomDrawThreeVec position;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
    RoomFxVec4 floorScale;
    RoomFxVec4 sourceFloorScale;
    s16 *args;
} RoomDrawThreeStack;

char *func_800C2B50(void);
void func_800C2EAC(u8 owner);
void func_800C3098(int depth);
void func_80079754(RoomDrawThreeVec *seed, RoomSpriteMatrix *matrix);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C3134(void *table, int step, void *packet);
void func_800C2FF0(int width, int height);
void func_800C3238(int mode);
void func_800C42A4(void *packet, RoomSpriteMatrix *matrix, int mode);
int func_80077CF4(int angle);
void func_800794C4(RoomFxSeed8 *seed, RoomSpriteMatrix *matrix);

extern RoomFxSeed8 D_8018EFF4;
extern unsigned char ROOMLIB_DRAW_THREE_PARTICLES_TABLE[];
extern unsigned char ROOMLIB_DRAW_THREE_PARTICLES_PACKET[];
extern s16 ROOMLIB_DRAW_THREE_PARTICLES_PRIMARY_ALPHA;
extern s16 ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_ALPHA;
extern s16 ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_HEIGHT;

void ROOMLIB_DRAW_THREE_PARTICLES_FUNC(void *unused, s16 *args, char *state) {
    RoomDrawThreeStack stack;
    register char *workState asm("$18");
    register unsigned int i asm("$20");
    RoomFxVec4 *sourceScale;
    register RoomSpriteMatrix *secondaryMatrix asm("$23");
    register char *positionCursor asm("$19");
    register char *shortCursor asm("$17");
    register char *element asm("$21");
    register int positionOffset asm("$22");
    register int scaleValue asm("$16");
    register void *arg0 asm("$4");
    register void *inputArg asm("$5");
    register void *outputArg asm("$6");
    register s16 *callArgs asm("$7");
    register char *positionSource asm("$2");
    char *clock;
    register int computedScale asm("$2");
    register int baseScale asm("$3");

    asm("" : "=r"(workState) : "0"(state));
    stack.args = args;
    clock = func_800C2B50();
    stack.seed = D_8018EFF4;
    i = 0;
    sourceScale = &stack.sourceScale;
    secondaryMatrix = &stack.secondaryMatrix;
    asm("" : "=r"(sourceScale), "=r"(secondaryMatrix)
        : "0"(sourceScale), "1"(secondaryMatrix));
    positionCursor = workState;
    shortCursor = workState;
    func_800C2EAC(*(u8 *)(clock + 0x2C));
    func_800C3098(0x10);

    do {
        element = workState + i;
        positionOffset = i << 3;
        if (*(s8 *)(element + 0x16) >= 0) {
            asm("" : "=r"(positionOffset) : "0"(positionOffset));
            arg0 = (void *)(positionOffset + 0x6C);
            asm("" : "=r"(arg0) : "0"(arg0));
            arg0 = workState + (int)arg0;
            func_80079754(arg0, &stack.primaryMatrix);
            func_80071A44(sourceScale, 0, 0x10);
            stack.sourceScale.x = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.sourceScale.y = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.sourceScale.z = *(u16 *)(shortCursor + 0x2A) >> 1;
            stack.scale = stack.sourceScale;
            func_80078CC4(&stack.primaryMatrix, &stack.scale);

            stack.primaryMatrix.t[0] = *(s16 *)(positionCursor + 0x3E) +
                                       *(s8 *)(workState + 0x0B);
            stack.primaryMatrix.t[1] = *(s16 *)(positionCursor + 0x42) +
                                       *(s8 *)(workState + 0x0C);
            stack.primaryMatrix.t[2] = *(s16 *)(positionCursor + 0x46) +
                                       *(s8 *)(workState + 0x0D);
            callArgs = stack.args;
            computedScale = *(u8 *)(element + 0x1E);
            asm volatile("" : "=r"(computedScale)
                : "0"(computedScale) : "memory");
            arg0 = ROOMLIB_DRAW_THREE_PARTICLES_TABLE;
            ROOMLIB_DRAW_THREE_PARTICLES_PRIMARY_ALPHA = computedScale;
            asm volatile("" ::: "memory");
            inputArg = (void *)(int)callArgs[1];
            asm volatile("" : "=r"(inputArg)
                : "0"(inputArg) : "memory");
            outputArg = ROOMLIB_DRAW_THREE_PARTICLES_PACKET;
            func_800C3134(arg0, (int)inputArg, outputArg);
            func_800C2FF0(0x40, 0x40);
            func_800C3238(2);
            arg0 = ROOMLIB_DRAW_THREE_PARTICLES_PACKET;
            asm("" : "=r"(arg0) : "0"(arg0));
            func_800C42A4(arg0, &stack.primaryMatrix, 1);

            if (*(s16 *)(workState + 8) == 1) {
                callArgs = stack.args;
                computedScale = func_80077CF4(callArgs[1] << 7) >> 3;
                baseScale = *(u16 *)(shortCursor + 0x32);
                computedScale += baseScale;
                asm("" : "=r"(computedScale) : "0"(computedScale));
                scaleValue = computedScale;
                asm("" : "=r"(scaleValue) : "0"(scaleValue));
                if (*(u16 *)(shortCursor + 0x2A) < (s16)computedScale) {
                    scaleValue = *(u16 *)(shortCursor + 0x2A);
                }

                positionSource =
                    (char *)((int)positionOffset + (int)workState);
                stack.position = *(RoomDrawThreeVec *)(positionSource + 0x6C);
                arg0 = &stack.position;
                inputArg = &stack.primaryMatrix;
                asm("" : "=r"(arg0), "=r"(inputArg)
                    : "0"(arg0), "1"(inputArg));
                scaleValue = (s16)scaleValue >> 1;
                asm("" : "=r"(scaleValue) : "0"(scaleValue));
                stack.position.z += *(u16 *)(shortCursor + 0x22) << 4;
                func_80079754(arg0, inputArg);
                func_80071A44(sourceScale, 0, 0x10);
                stack.sourceScale.x = scaleValue;
                stack.sourceScale.y = scaleValue;
                stack.sourceScale.z = scaleValue;
                func_80078CC4(&stack.primaryMatrix, sourceScale);
                stack.primaryMatrix.t[0] = *(s16 *)(positionCursor + 0x3E) +
                                           *(s8 *)(workState + 0x0B);
                stack.primaryMatrix.t[1] = *(s16 *)(positionCursor + 0x42) +
                                           *(s8 *)(workState + 0x0C);
                stack.primaryMatrix.t[2] = *(s16 *)(positionCursor + 0x46) +
                                           *(s8 *)(workState + 0x0D);
                arg0 = ROOMLIB_DRAW_THREE_PARTICLES_PACKET;
                func_800C42A4(arg0, &stack.primaryMatrix, 1);

                func_800794C4(&stack.seed, secondaryMatrix);
                func_80071A44(&stack.sourceFloorScale, 0, 0x10);
                stack.sourceFloorScale.x = *(u16 *)(shortCursor + 0x2A);
                stack.sourceFloorScale.y = *(u16 *)(shortCursor + 0x2A);
                stack.sourceFloorScale.z = *(u16 *)(shortCursor + 0x2A);
                stack.floorScale = stack.sourceFloorScale;
                func_80078CC4(secondaryMatrix, &stack.floorScale);
                computedScale = *(s16 *)(positionCursor + 0x3E);
                baseScale = ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_HEIGHT;
                asm volatile("" : "=r"(computedScale), "=r"(baseScale)
                    : "0"(computedScale), "1"(baseScale));
                arg0 = (void *)0x20;
                asm("" : "=r"(arg0) : "0"(arg0));
                stack.secondaryMatrix.t[0] = computedScale;
                stack.secondaryMatrix.t[1] = baseScale;
                computedScale = *(s16 *)(positionCursor + 0x46);
                asm volatile("" : "=r"(computedScale)
                    : "0"(computedScale));
                inputArg = (void *)0x20;
                asm("" : "=r"(inputArg) : "0"(inputArg));
                stack.secondaryMatrix.t[2] = computedScale;
                computedScale = *(u8 *)(element + 0x1E);
                asm volatile("" : "=r"(computedScale)
                    : "0"(computedScale) : "memory");
                scaleValue =
                    (int)&ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_ALPHA;
                asm("" : "=r"(scaleValue) : "0"(scaleValue));
                *(s16 *)scaleValue = (unsigned int)computedScale >> 1;
                func_800C2FF0((int)arg0, (int)inputArg);
                func_800C3238(3);
                func_800C42A4((char *)scaleValue - 0xA,
                              secondaryMatrix, 0);
            }
        }
        positionCursor += 0x10;
        i++;
        shortCursor += 2;
    } while (i < 3);
}

#undef ROOMLIB_DRAW_THREE_PARTICLES_FUNC
#undef ROOMLIB_DRAW_THREE_PARTICLES_TABLE
#undef ROOMLIB_DRAW_THREE_PARTICLES_PACKET
#undef ROOMLIB_DRAW_THREE_PARTICLES_PRIMARY_ALPHA
#undef ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_ALPHA
#undef ROOMLIB_DRAW_THREE_PARTICLES_FLOOR_HEIGHT
