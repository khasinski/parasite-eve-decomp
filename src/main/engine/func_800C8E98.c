extern char D_800E09B4[];
extern char D_800E098C[];
extern char D_800E09C8[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C8F08(void *obj);

int func_800C8E98(void *obj) {
    int status = func_800C251C(obj, D_800E09B4);

    status |= func_800C2758(obj, D_800E098C, D_800E09C8);
    if (status == -1) {
        func_800C8F08(obj);
    }
    return 0;
}
