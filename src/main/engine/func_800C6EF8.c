#include "common.h"
extern u32 D_800E2370[];

void func_800C6EF8(char *data) {
    int i = 0;
    u32 *src = (u32 *)(data + *(u16 *)(data + 0x8));
    u32 *dst = D_800E2370;

    while (i < *(u16 *)(data + 0xA)) {
        i++;
        *dst++ = *src++;
    }
}
