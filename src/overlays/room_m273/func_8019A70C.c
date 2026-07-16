extern short D_8019AF9A;

short *func_8019A70C(void *arg0, short arg1) {
    short *ptr = &D_8019AF9A;

    *ptr = arg1;
    return ptr - 0x17;
}
