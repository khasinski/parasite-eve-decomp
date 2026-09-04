#ifndef ROOMLIB_DRAW_SCALED_SPRITE_FUNC
#error "ROOMLIB_DRAW_SCALED_SPRITE_FUNC must name the room entry point"
#endif

typedef struct RoomScaledSpriteStack {
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomScaledSpriteStack;

char *func_800C2B50(void);
void func_800794C4(void *seed, RoomSpriteMatrix *matrix);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
void func_80078CC4(RoomSpriteMatrix *matrix, RoomFxVec4 *scale);
void func_800C6D5C(void *packet, int arg1, int arg2);
int func_80077A64(int arg0, int arg1, int arg2, int arg3);
int func_80077AA4(int arg0, int arg1);
void func_800C6EC0(int arg0, int arg1);
void func_800C6ED8(int arg0);
void func_800C6EF8(void *packet);
void func_800C7098(void *packet, int arg1, int arg2, int arg3);
void func_800C6FA0(void *packet, int depth);
void func_800C71E4(void *packet, RoomSpriteMatrix *matrix);
void func_800C6F4C(void *packet);

extern void *ROOMLIB_SCALED_SPRITE_PACKET;

void ROOMLIB_DRAW_SCALED_SPRITE_FUNC(char *object, void *unused, char *state) {
    RoomScaledSpriteStack stack;
    register char *entity asm("$16");
    register char *workState asm("$17");
    register char *clock asm("$18");
    register int callArg0 asm("$4");
    register int callArg1 asm("$5");
    int handleA;
    int handleB;

    entity = object;
    workState = state;
    clock = func_800C2B50();
    func_800794C4(workState + 8, &stack.matrix);

    stack.matrix.t[0] = *(s16 *)(*(char **)(entity + 8) + 0x2A);
    stack.matrix.t[1] = *(s16 *)(*(char **)(entity + 8) + 0x2E);
    stack.matrix.t[2] = *(s16 *)(*(char **)(entity + 8) + 0x32);

    func_80071A44(&stack.sourceScale, 0, 0x10);
    stack.sourceScale.x = *(s16 *)(workState + 0x10);
    stack.sourceScale.y = 0xC8;
    stack.sourceScale.z = *(s16 *)(workState + 0x10);
    stack.scale = stack.sourceScale;
    func_80078CC4(&stack.matrix, &stack.scale);

    func_800C6D5C(ROOMLIB_SCALED_SPRITE_PACKET, 0, 0);
    if (*(int *)(clock + 0x24) == 0) {
        handleA = func_80077A64(0, 1, 0x340, 0x100);
        callArg0 = 0;
        callArg1 = 0x1D7;
        asm("" : : "r"(callArg0), "r"(callArg1));
        handleA = (u16)handleA;
        handleB = func_80077AA4(callArg0, callArg1);
        func_800C6EC0((u16)handleA, (u16)handleB);
    }
    if (*(int *)(clock + 0x24) == 1) {
        handleA = func_80077A64(0, 1, 0x340, 0x160);
        callArg0 = 0;
        callArg1 = 0x1DB;
        asm("" : : "r"(callArg0), "r"(callArg1));
        handleA = (u16)handleA;
        handleB = func_80077AA4(callArg0, callArg1);
        func_800C6EC0((u16)handleA, (u16)handleB);
    }
    func_800C6ED8(1);
    func_800C6EF8(ROOMLIB_SCALED_SPRITE_PACKET);
    func_800C7098(ROOMLIB_SCALED_SPRITE_PACKET, 0x60, 0x10, 0x80);
    func_800C6FA0(ROOMLIB_SCALED_SPRITE_PACKET, *(u16 *)(workState + 0x12));
    func_800C71E4(ROOMLIB_SCALED_SPRITE_PACKET, &stack.matrix);
    func_800C6F4C(ROOMLIB_SCALED_SPRITE_PACKET);
}

#undef ROOMLIB_DRAW_SCALED_SPRITE_FUNC
#undef ROOMLIB_SCALED_SPRITE_PACKET
