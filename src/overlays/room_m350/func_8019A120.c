extern short D_8019A8B8;

short *func_8019A120(void *arg0, short arg1) {
    register short *ptr asm("v0") = &D_8019A8B8;

    *ptr = arg1;
    return ptr - 0x24;
}
