int func_80192808(char *obj) {
    int *state;

    (*(char **)(obj + 8))[0x25A] = 0;
    (*(char **)(obj + 8))[0x25B] = 0;
    state = *(int **)(obj + 0x10);
    obj[0] = 4;
    if (state != 0) {
        *state = 0;
    }

    return 0;
}
