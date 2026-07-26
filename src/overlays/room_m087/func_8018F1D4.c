typedef int s32;

extern void *D_80193534;

void **func_800C22F8(void);

s32 func_8018F1D4(void) {
    void **slot;

    slot = func_800C22F8();
    *slot = &D_80193534;
    return 0;
}
