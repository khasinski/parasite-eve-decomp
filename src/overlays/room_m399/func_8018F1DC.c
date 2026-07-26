typedef int s32;

extern void *D_80194208;

void **func_800C22F8(void);

s32 func_8018F1DC(void) {
    void **slot;

    slot = func_800C22F8();
    *slot = &D_80194208;
    return 0;
}
