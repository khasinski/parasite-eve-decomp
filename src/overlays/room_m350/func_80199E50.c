typedef int s32;

extern char D_8019A854;
extern char D_8019A86E;
extern char **D_800F32D0;
extern char **D_800F33E0;

extern void func_8019A134(void);
extern s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void (*arg3)(void));
extern char *func_800CE610(void *arg0);

s32 func_80199E50(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0[2], 4, 2, func_8019A134);
    } else if (D_8019A86E != 0) {
        return 2;
    } else if (D_8019A854 != 0) {
        obj = func_800CE610(D_800F33E0[2]);
        if (obj != 0) {
            *(s32 *)(obj + 0) = *(s32 *)(D_800F32D0[2] + 0x238) + 0xF4;
            D_8019A854 = 0;
        }
    }

    return 0;
}
