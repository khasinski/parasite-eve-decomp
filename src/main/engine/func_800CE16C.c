extern char D_800E0FCC[];
extern char D_800E0FB4[];
extern char D_800E0FD8[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CE1DC(void *obj);

int func_800CE16C(void *obj) {
    int status = func_800C251C(obj, D_800E0FCC);

    status |= func_800C2758(obj, D_800E0FB4, D_800E0FD8);
    if (status == -1) {
        func_800CE1DC(obj);
    }
    return 0;
}
