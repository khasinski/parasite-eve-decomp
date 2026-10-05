void func_8018F18C(void);

void func_8018F160(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (slot[0xE] == 0x10) {
        *(void (**)(void))(obj + 0xC) = func_8018F18C;
    }
}
