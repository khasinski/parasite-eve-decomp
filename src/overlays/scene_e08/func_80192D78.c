void func_80192D78(void *arg0, void *arg1, char *arg2) {
    char *end = arg2 + 0x20;

    do {
        *arg2++ = 0;
    } while (arg2 < end);
}
