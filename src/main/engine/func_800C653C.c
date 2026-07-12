int func_800C62DC(void *from, void *to);

int func_800C653C(void *arg0, char *arg1) {
    int first = func_800C62DC(arg0, arg1);

    return first | func_800C62DC(arg0, arg1 + 8);
}
