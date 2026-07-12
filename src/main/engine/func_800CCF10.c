extern char D_800E0E78[];
extern char D_800E0E48[];
extern char D_800E0E90[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CCF80(void *obj);

int func_800CCF10(void *obj) {
    int status = func_800C251C(obj, D_800E0E78);

    status |= func_800C2758(obj, D_800E0E48, D_800E0E90);
    if (status == -1) {
        func_800CCF80(obj);
    }
    return 0;
}
