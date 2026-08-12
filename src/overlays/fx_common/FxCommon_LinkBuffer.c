#include "fx_common.h"

void func_8019BF8C(void **buffer)
{
    if (buffer == (void **)&D_8019C1F8) {
        *buffer = D_800B0E4C;
    } else {
        *buffer = D_800B0E4C + 1;
    }
}
