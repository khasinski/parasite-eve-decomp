typedef int s32;
typedef short s16;

extern void *func_800C2B10(s32 arg0);
extern void func_800C2B90(s32 arg0, s32 arg1, void *arg2, void *arg3);

extern char D_80196CB8;
extern char D_80196C7C;

void func_8018F60C(s32 arg0, char *arg1, char *arg2) {
    s32 *slot;

    slot = func_800C2B10(4);
    *slot = *(s16 *)(arg2 + 2);
    func_800C2B90(arg0, 2, &D_80196CB8, &D_80196C7C);
    arg1[1] = 2;
}
