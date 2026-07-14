typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

typedef struct Vec4i {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Vec4i;

typedef struct StackFrame {
    Vec4i sp10;
    s32 pad20;
    s32 sp24[3];
    Vec4i sp30;
    Vec4i sp40;
} StackFrame;

extern s16 D_80192E1A;

extern char *func_800C2B50(void);
extern void func_800C2EAC(s32 arg0);
extern void func_800C2FF0(s32 arg0, s32 arg1);
extern void func_800C3098(s32 arg0);
extern void func_800C3238(s32 arg0);
extern void func_800794C4(void *arg0, void *arg1);
extern void func_80071A44(void *arg0, s32 arg1, s32 arg2);
extern void func_80078CC4(void *arg0, void *arg1);
extern void func_800C42A4(void *arg0, void *arg1, s32 arg2);

void func_80190C44(s32 arg0, s32 arg1, char *arg2) {
    StackFrame stack;
    s16 *ptr;

    func_800C2EAC((u8)func_800C2B50()[0x44]);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800794C4(arg2 + 8, &stack.sp10);
    func_80071A44(&stack.sp40, 0, 0x10);

    {
        register s32 x asm("$2") = *(s16 *)(arg2 + 0x10);
        stack.sp40.x = x;
    }
    {
        register s32 y asm("$3") = *(s16 *)(arg2 + 0x10);
        stack.sp40.z = 0x1000;
        stack.sp40.y = y;
    }
    stack.sp30 = stack.sp40;
    func_80078CC4(&stack.sp10, &stack.sp30);

    ptr = &D_80192E1A;
    *ptr = *(u16 *)(arg2 + 0x12);
    stack.sp24[0] = *(s16 *)(arg2 + 0);
    stack.sp24[1] = *(s16 *)(arg2 + 2);
    stack.sp24[2] = *(s16 *)(arg2 + 4);
    func_800C42A4(ptr - 5, &stack.sp10, 1);
}
