extern char D_800E09B4[];
extern char D_800E098C[];
extern char D_800E09C8[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C8F08(void *obj);

int func_800C8E98(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E09B4);
    second = func_800C2758(obj, D_800E098C, D_800E09C8);
    status = first | second;
    if (status == -1) {
        func_800C8F08(obj);
    }
    return 0;
}
