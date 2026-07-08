extern unsigned char g_AyaInventorySlotCount[];

int Inv_GetBonusSlotCount(void);

typedef unsigned char u8;

#include "pe1/inventory.h"

extern u8 g_InvItemSlotArray[];

int Inv_GetAyaSlotLimit(void) {
    if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() >= 0x33) {
        return 0x32;
    }

    return g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
}

void Inv_ClearEquipFlagForKind(InvItemSlot *obj) {
    u8 *base;
    u8 *entry;
    u8 *end;
    int kind;
    int is_kind_9;
    int kind_9;
    int one;

    if (obj != 0) {
        kind = obj->kind;
        base = g_InvItemSlotArray;
        kind ^= 9;
        is_kind_9 = (unsigned int)kind < 1;
        kind = (int)(base + 0x1000);
        if (base < (u8 *)kind) {
            kind_9 = 9;
            one = 1;
            entry = base + 5;
            end = base + 0x1005;

            do {
                if (entry[1] != kind_9) {
                    if (is_kind_9 == one) {
                        entry += 0x20;
                        goto loop_test;
                    }
                } else if (is_kind_9 == 0) {
                    entry += 0x20;
                    goto loop_test;
                }

                entry[0] &= 0xEF;
                entry += 0x20;
loop_test:
            } while (entry < end);
        }

        obj->flags |= 0x10;
    }
}
