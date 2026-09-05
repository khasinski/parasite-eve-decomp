#include "common.h"
#include "pe1/field_engine_state.h"
extern u8 *D_800F34F4;
extern char *D_800E2248;

int func_800C2E08(void) {
    int i;
    int offset;
    int result;
    u32 andMask;
    u32 xorMask;
    FieldEngSlot *entry;

    result = 0;
    i = 0;
    andMask = 0xFFFF0000;
    xorMask = 0x01000000;
    offset = 0;
    for (; i < 0x40; i++, offset += 6) {
        if (((FieldEngSlot *)(offset + (int)D_800F34F4))->flag != 0) {
            u32 check;
            entry = (FieldEngSlot *)((u16)i * 6 + (int)D_800F34F4);
            entry->flag = 0;
            D_800E2248[6]--;
            check = *(u32 *)(D_800E2248 + 4) & andMask;
            check = check == xorMask;
            asm("" : "=r"(check) : "0"(check));
            result |= -check;
        }
    }

    return result;
}
