int RoomLib_TickMain(char *ctx) {
    unsigned int mask = 0xFFFEFFFF;
    register char *obj asm("v1") = *(char **)(ctx + 8);
    char *window;
    char *entry;

    ctx[0] = 4;
    ctx[3] = 0;
    *(unsigned int *)(obj + 0x98) &= mask;

    obj = *(char **)(ctx + 8);
    *(unsigned short *)(obj + 0x250) &= 0xFBFF;

    window = *(char **)(*(char **)(ctx + 8) + 0x18C);
    if (window != 0) {
        entry = *(char **)window;
        if (entry != 0) {
            *(char *)(*(char **)(entry + 0x18)) = 4;
        }
    }

    return 0;
}
