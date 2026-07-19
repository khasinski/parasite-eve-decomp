/* MASPSX_FLAGS: --label-branch-delay */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;

s32 func_8018FEB8(char *obj) {
    char *sub = obj + 0xC;
    s8 four = 4;

    obj[0] = four;
    obj[3] = 0;
    if (obj[0xB8] == 0) {
        if (*(s16 *)(obj + 0x14) != 0) {
            char *d = **(char ***)(obj + 8);
            if (d != 0) {
                char *e = *(char **)(d + 0x18);
                e[0] = four;
                obj[0] = four;
            }
        }
    } else {
        char *d = **(char ***)(obj + 8);
        if (d != 0) {
            char *e = *(char **)(d + 0x18);
            if (e[0] == 1) {
                e[0] = four;
            }
        }
    }
    {
        s32 *q = *(s32 **)(sub + 4);
        if (q != 0) {
            *q = 0;
        }
    }
    return 0;
}
