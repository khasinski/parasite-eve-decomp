extern char D_800E0BA0[];
extern char D_800E0B68[];
extern char D_800E0BBC[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CA798(void *obj);

int func_800CA728(void *obj) {
    int status = func_800C251C(obj, D_800E0BA0);

    status |= func_800C2758(obj, D_800E0B68, D_800E0BBC);
    if (status == -1) {
        func_800CA798(obj);
    }
    return 0;
}
