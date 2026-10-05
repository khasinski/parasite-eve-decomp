int func_80193250(char *ctx) {
    char *node;

    ctx[0] = 4;
    ctx[3] = 0;

    *(int *)(*(char **)(ctx + 8) + 0x98) &= 0xFFFEFFFF;
    *(unsigned short *)(*(char **)(ctx + 8) + 0x250) &= 0xFBFF;

    node = *(char **)(*(char **)(ctx + 8) + 0x18C);
    if (node != 0) {
        node = *(char **)node;
        if (node != 0) {
            (*(char **)(node + 0x18))[0] = 4;
        }
    }

    return 0;
}
