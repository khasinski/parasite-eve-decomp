#include "common.h"
extern void *D_8019437C;

void **func_800C22F8(void);

s32 func_8018F1E8(void) {
    void **slot;

    slot = func_800C22F8();
    *slot = &D_8019437C;
    return 0;
}
