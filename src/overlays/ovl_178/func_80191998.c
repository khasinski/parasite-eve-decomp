typedef signed short s16;

void func_80191998(void *arg0, void *arg1, char *arg2) {
    unsigned int i;

    for (i = 0; i < 16; i++) {
        *(s16 *)(arg2 + 0x1C0) = 0;
        arg2 += 2;
    }
}
