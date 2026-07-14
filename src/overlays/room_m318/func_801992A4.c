typedef int s32;
typedef short s16;

extern s16 *D_800E2368;
extern char **D_8009D254;
extern s32 D_800E27EC;

s32 func_801992A4(s32 arg0, void *arg1, s32 *arg2) {
    char *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        D_800E2368[10] = 0;
        D_800E2368[11] = 0;
        {
            register char *actor0 asm("$4") = *D_8009D254;
            register s32 mask asm("$5") = 0xEFFFFFFF;
            *(s32 *)(actor0 + 0x4C) &= mask;
        }
        return 0;
    } else {
        actor = *D_8009D254;
        if (*(s32 *)(actor + 0x4C) & 0x10000000) {
            D_800E2368[10] = 1;
            D_800E2368[11] = 1;
            return 1;
        }
        if (D_800E27EC < *arg2) {
            return 0;
        }
        D_800E2368[10] = 1;
        D_800E2368[11] = 0;
        return 1;
    }
}
