extern int FieldEng_GetStatus(void);
extern int func_800C251C(void *arg0, void *arg1);
extern int func_800C2758(void *arg0, void *arg1, void *arg2);

extern char D_801954BC[];
extern char D_801954F4[];
extern char D_80195510[];

extern int func_8018F358(void *arg0);

int func_8018F2CC(void *arg0) {
    int result;

    if (FieldEng_GetStatus() == 3) {
        result = func_800C251C(arg0, D_801954F4);
        result |= func_800C2758(arg0, D_801954BC, D_80195510);
    } else {
        result = -1;
    }

    if (result == -1) {
        func_8018F358(arg0);
    }

    return 0;
}
