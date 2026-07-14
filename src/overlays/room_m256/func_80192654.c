void func_80192680(void);

void func_80192654(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (slot[0xE] == 0xB) {
        *(void (**)(void))(obj + 0xC) = func_80192680;
    }
}
