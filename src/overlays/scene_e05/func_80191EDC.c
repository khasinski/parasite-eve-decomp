typedef signed int s32;

extern s32 D_80192140;
extern s32 D_80192144;
extern s32 D_80192148;
extern s32 D_8019214C;
extern s32 D_80192150;
extern s32 D_80192154;
extern s32 D_80192158;
extern s32 D_8019215C;

int func_80191EDC(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (arg0 == 1) {
        goto case1;
    }
    if (arg0 == 0) {
        goto case0;
    }
    if (arg0 == 2) {
        goto case2;
    }
    goto set54;

case0:
    D_80192140 = arg1;
    D_80192144 = arg2;
    D_8019215C = arg3;
    if (arg3 != 0) {
        goto done;
    }
    D_8019215C = 8;
    goto set54;

case1:
    D_80192148 = arg1;
    D_8019214C = arg2;
    D_80192158 = arg3;
    if (arg3 != 0) {
        goto done;
    }
    D_80192158 = 10;
    goto set54;

case2:
    D_80192150 = arg1;

set54:
    D_80192154 = arg2;
done:
    return 0;
}
