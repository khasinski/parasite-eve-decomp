typedef signed char s8;
typedef unsigned char u8;
typedef int s32;

s32 func_80079754(void *arg0, void *arg1);
s32 func_800785D4(void *arg0, void *arg1, void *arg2);

extern char D_8019B31C[];
extern char D_8019B2FC[];
extern s32 D_8019B310;
extern s32 D_8019B314;
extern s32 D_8019B318;

extern char D_80192688[];

s32 func_8019259C(char *obj) {
    *(char **)(obj + 0xC) = D_80192688;
    obj[0x1A] = 0;
    obj[0x44] = 0;
    obj[0x3] = 0xFF;
    func_80079754(D_8019B31C, (void *)0x1F800028);
    func_800785D4(D_8019B2FC, (void *)0x1F800028, obj + 0x1C);
    *(s32 *)(obj + 0x30) = D_8019B310;
    *(s32 *)(obj + 0x34) = D_8019B314;
    *(s32 *)(obj + 0x38) = D_8019B318;
    return 0;
}
