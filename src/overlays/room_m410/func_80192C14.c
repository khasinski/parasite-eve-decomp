extern char D_80192D3C[];
extern int D_80192D40;
extern int D_80192D44;

void func_80192C14(int arg0, int arg1, int arg2, int arg3) {
    volatile int *dst = (volatile int *)D_80192D3C;

    dst[0] = arg1;
    D_80192D40 = arg2;
    D_80192D44 = arg3;
}
