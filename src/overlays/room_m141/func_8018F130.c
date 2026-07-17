extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_80191FDC[];
extern char D_80192014[];
extern char D_80192030[];

extern int func_8018F1BC(void *arg0);

int func_8018F130(void *arg0) {
    int result;

    if (FieldEng_GetStatus() == 3) {
        result = func_800C251C(arg0, D_80192014);
        result |= func_800C2758(arg0, D_80191FDC, D_80192030);
    } else {
        result = -1;
    }

    if (result == -1) {
        func_8018F1BC(arg0);
    }

    return 0;
}
