void func_80192664(void);

void func_80192638(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (slot[0xE] == 0x10) {
        *(void (**)(void))(obj + 0xC) = func_80192664;
    }
}
