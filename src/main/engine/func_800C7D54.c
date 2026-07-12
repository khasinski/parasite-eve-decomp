extern char D_800E0840[];
extern char D_800E0808[];
extern char D_800E085C[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C7DC4(void *obj);

int func_800C7D54(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0840);
    second = func_800C2758(obj, D_800E0808, D_800E085C);
    status = first | second;
    if (status == -1) {
        func_800C7DC4(obj);
    }
    return 0;
}
