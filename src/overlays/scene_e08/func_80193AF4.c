void **func_800C22F8(void);

extern char D_80198A64[];

int func_80193AF4(void) {
    void **slot = func_800C22F8();

    *slot = D_80198A64;
    return 0;
}
