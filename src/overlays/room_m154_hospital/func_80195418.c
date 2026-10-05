extern int D_80195738;
extern int D_8019573C;
extern int D_80195740;
extern int D_80195744;
extern int D_80195748;
extern int D_8019574C;
extern int D_80195750;
extern int D_80195754;

int func_80195418(int mode, int a, int b, int c) {
    if (mode == 1) {
        goto mode1;
    }
    if (mode == 0) {
        goto mode0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto done;

mode0:
    D_80195738 = a;
    D_8019573C = b;
    D_80195754 = c;
    if (c == 0) {
        D_80195754 = 8;
    }
    goto done;

mode1:
    D_80195740 = a;
    D_80195744 = b;
    D_80195750 = c;
    if (c == 0) {
        D_80195750 = 10;
    }
    goto done;

mode2:
    D_80195748 = a;
    D_8019574C = b;

done:
    return 0;
}
