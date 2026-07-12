extern char D_800E0F38[];
extern char D_800E0F18[];
extern char D_800E0F48[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CD960(void *obj);

int func_800CD8F0(void *obj) {
    int status = func_800C251C(obj, D_800E0F38);

    status |= func_800C2758(obj, D_800E0F18, D_800E0F48);
    if (status == -1) {
        func_800CD960(obj);
    }
    return 0;
}
