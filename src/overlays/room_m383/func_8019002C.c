typedef int s32;
typedef short s16;
typedef unsigned short u16;

typedef struct Vec4i {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Vec4i;

typedef struct StackFrame {
    s16 h10;
    s16 h12;
    s16 h14;
    s16 h16;
    s16 h18;
    s16 h1A;
    s16 h1C;
    s16 h1E;
    s16 h20;
    s32 sp24[3];
    Vec4i sp30;
} StackFrame;

extern s16 D_8019745A;
extern Vec4i D_8018EFFC;

extern void func_800C2EAC(s32 arg0);
extern void func_800C3098(s32 arg0);
extern void func_800C2FF0(s32 arg0, s32 arg1);
extern void func_800C3238(s32 arg0);
extern void func_80078CC4(void *arg0, void *arg1);
extern void func_800C42A4(void *arg0, void *arg1, s32 arg2);

void func_8019002C(s32 arg0, s32 arg1, char *arg2) {
    StackFrame stack;
    s16 *ptr;

    func_800C2EAC(0);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    stack.h20 = 0x1000;
    stack.h18 = 0x1000;
    stack.h10 = 0x1000;
    stack.sp24[2] = 0;
    stack.sp24[1] = 0;
    stack.sp24[0] = 0;
    stack.h1E = 0;
    stack.h1C = 0;
    stack.h1A = 0;
    stack.h16 = 0;
    stack.h14 = 0;
    stack.h12 = 0;

    ptr = &D_8019745A;
    *ptr = *(u16 *)(arg2 + 4);
    stack.sp24[0] = *(s16 *)(arg2 + 8);
    stack.sp24[1] = *(s16 *)(arg2 + 0xA);
    stack.sp24[2] = *(s16 *)(arg2 + 0xC);
    stack.sp30 = D_8018EFFC;
    func_80078CC4(&stack, &stack.sp30);
    func_800C42A4(ptr - 5, &stack, 1);
}
