typedef int s32;

extern char D_80192BE8;

void **func_800C22F8(void);

s32 func_80100054(void) {
    void **slot = func_800C22F8();

    *slot = &D_80192BE8;
    return 0;
}
