void RoomLib_TickMain(void *ctx);

void func_8018FD14(void *ctx) {
    char *obj = *(char **)((char *)ctx + 8);
    char *window = *(char **)(obj + 0x18C);
    unsigned int limit = (unsigned char)window[0xF];

    limit--;
    if (*(unsigned short *)(window + 0x1A) >= limit) {
        RoomLib_TickMain(ctx);
    }
}
