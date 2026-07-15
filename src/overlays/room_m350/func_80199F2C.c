typedef int s32;
typedef unsigned short u16;

extern char D_8019A856;
extern char D_8019A86E;
extern u16 D_800942EC;
extern u16 D_8019A860;
extern u16 D_8019A862;
extern char **D_800F33E0;

extern void func_8019A21C(void);
extern s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void (*arg3)(void));
extern char *func_800CE610(void *arg0);

s32 func_80199F2C(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0[2], 8, 6, func_8019A21C);
    } else if (D_8019A86E != 0) {
        return 2;
    } else if (D_8019A856 != 0) {
        obj = func_800CE610(D_800F33E0[2]);
        if (obj != 0) {
            *(u16 *)(obj + 0) = D_8019A860;
            *(u16 *)(obj + 2) = D_800942EC;
            *(u16 *)(obj + 4) = D_8019A862;
            D_8019A856 = 0;
        }
    }

    return 0;
}
