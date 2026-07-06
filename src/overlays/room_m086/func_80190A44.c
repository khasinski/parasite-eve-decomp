extern int D_80190B74;
extern int D_80190B78;
extern int D_80190B7C;
extern int D_80190B80;

int *func_80190A44(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        D_80190B74 = arg1;
        D_80190B78 = arg2;
        D_80190B7C = arg3;
    } else if (arg0 == 2) {
        D_80190B80 = arg1;
    }
    return &D_80190B74;
}
