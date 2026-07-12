extern char D_800E0D24[];
extern char D_800E0CEC[];
extern char D_800E0D40[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CBFA4(void *obj);

int func_800CBF34(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0D24);
    second = func_800C2758(obj, D_800E0CEC, D_800E0D40);
    status = first | second;
    if (status == -1) {
        func_800CBFA4(obj);
    }
    return 0;
}
