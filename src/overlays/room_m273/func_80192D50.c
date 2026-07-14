void func_80192D8C(void);

void func_80192D50(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (*(unsigned short *)(slot + 0x16) >= 0x3B) {
        func_80192D8C();
    }
}
