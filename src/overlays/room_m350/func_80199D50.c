typedef int s32;
typedef unsigned char u8;

extern char *D_8009D254;
extern char D_8019A82E;
extern char D_8019A830;
extern char **D_800F33E0;

extern void func_80196C64(void);
extern s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, void (*arg3)(void));
extern char *func_800CE610(void *arg0);

s32 func_80199D50(s32 arg0) {
    char *obj;
    char *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        obj = D_800F33E0[2];
        return func_800CE560(obj, 8, 2, func_80196C64);
    } else if (D_8019A82E != 0) {
        return 2;
    } else if (D_8019A830 != 0) {
        obj = func_800CE610(D_800F33E0[2]);
        if (obj != 0) {
            actor = D_8009D254;
            *(short *)(obj + 0) = *(s32 *)(actor + 0x1FC);
            *(short *)(obj + 2) = *(s32 *)(actor + 0x200) - 0x140;
            {
                s32 value = *(s32 *)(actor + 0x204);
                *(short *)(obj + 4) = value;
            }
            D_8019A830 = 0;
        }
    }

    return 0;
}
