extern int D_80192648;
extern int D_8019264C;
extern int D_80192650;
extern int D_80192654;
extern int D_80192658;
extern int D_8019265C;
extern int D_80192660;
extern int D_80192664;

int func_801923A4(int mode, int arg1, int arg2, int arg3) {
    if (mode == 1) {
        goto case1;
    }
    if (mode == 0) {
        goto case0;
    }
    if (mode == 2) {
        goto case2;
    }
    goto done;

case0:
    D_80192648 = arg1;
    D_8019264C = arg2;
    D_80192664 = arg3;
    if (arg3 == 0) {
        D_80192664 = 8;
    }
    goto done;

case1:
    D_80192650 = arg1;
    D_80192654 = arg2;
    D_80192660 = arg3;
    if (arg3 == 0) {
        D_80192660 = 10;
    }
    goto done;

case2:
    D_80192658 = arg1;
    D_8019265C = arg2;

done:
    return 0;
}
