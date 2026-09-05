extern char SceneE08_CallbackDone[];

void func_8019838C(char *obj) {
    char *link;

    link = *(char **)(obj + 8);
    if (*(unsigned char *)(link + 0xE) == *(signed char *)(obj + 0x16)) {
        *(void **)(obj + 0xC) = SceneE08_CallbackDone;
    }
}
