extern int D_80192180;
extern int D_80192184;
extern int D_80192188;
extern int D_8019218C;
extern int D_80192190;
extern int D_80192194;
extern int D_80192198;
extern int D_8019219C;

int func_80191F38(int arg0, int arg1, int arg2, int arg3) {
    int value = 1;

    if (arg0 == value) {
        goto case_1;
    }
    value = 2;
    if (arg0 == 0) {
        goto case_0;
    }
    if (arg0 == value) {
        goto case_2;
    }
    goto ret;

case_0:
    D_80192180 = arg1;
    D_80192184 = arg2;
    D_8019219C = arg3;
    if (arg3 == 0) {
        D_8019219C = 8;
    }
    goto ret;

case_1:
    D_80192188 = arg1;
    D_8019218C = arg2;
    D_80192198 = arg3;
    if (arg3 == 0) {
        D_80192198 = 10;
    }
    goto ret;

case_2:
    D_80192190 = arg1;
    D_80192194 = arg2;
ret:
    return 0;
}
