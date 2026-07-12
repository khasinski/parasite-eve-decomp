extern char D_800E0AA4[];
extern char D_800E0A84[];
extern char D_800E0AB4[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C9C00(void *obj);

int func_800C9B90(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0AA4);
    second = func_800C2758(obj, D_800E0A84, D_800E0AB4);
    status = first | second;
    if (status == -1) {
        func_800C9C00(obj);
    }
    return 0;
}
