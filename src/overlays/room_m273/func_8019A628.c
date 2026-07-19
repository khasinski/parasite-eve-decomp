typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;

extern s32 D_800E27EC;
extern u8 D_8019AF6B;
extern u16 D_8019AEFC;
extern u16 D_8019AEFE;
extern u16 D_8019AF00;
extern char **D_800F33E0;

extern void func_8019A720(void);
extern s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void (*arg3)(void));
extern char *func_800CE610(void *arg0);

s32 func_8019A628(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0[2], 8, 5, func_8019A720);
    }

    if (D_8019AF6B != 0) {
        return 2;
    }
    if ((D_800E27EC & 3) != 0) {
        return 0;
    }

    obj = func_800CE610(D_800F33E0[2]);
    if (obj != 0) {
        *(u16 *)(obj + 0) = D_8019AEFC;
        *(u16 *)(obj + 2) = D_8019AEFE;
        *(u16 *)(obj + 4) = D_8019AF00;
        *(u16 *)(obj + 6) = 0;
    }

    return 0;
}
