typedef int s32;
typedef short s16;

extern void *func_800C2B10(s32 arg0);
extern void func_800C2B90(s32 arg0, s32 arg1, void *arg2, void *arg3);

extern char D_8018FF70;
extern char D_8018FF34;

void func_8018F3C4(s32 arg0, char *arg1, char *arg2) {
    s32 *slot;

    slot = func_800C2B10(4);
    *slot = *(s16 *)(arg2 + 2);
    func_800C2B90(arg0, 2, &D_8018FF70, &D_8018FF34);
    arg1[1] = 2;
}
