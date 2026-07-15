int func_80192638(char *obj, int mode, int event, int arg3) {
    if (event == 0x19 && mode == 1) {
        *(int *)(obj + 0x10) = arg3;
        *(int *)arg3 = mode;
    }

    return 0;
}
