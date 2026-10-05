#include "common.h"
extern void *D_801935F8;

void **func_800C22F8(void);

s32 func_8018F1D4(void) {
    void **slot;

    slot = func_800C22F8();
    *slot = &D_801935F8;
    return 0;
}
