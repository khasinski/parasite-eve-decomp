#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_InvCategoryBaseItemId;
extern u16 g_BattleCountTable[];
extern u16 g_InvCategoryItemTable[][16];
extern u16 *g_InvActiveListOverride;
extern int g_InvOverrideSlotLimit;
extern int g_MenuBattleCount;

void Inv_RebuildSelectableMask(void);

int Inv_LoadWayneItemsAsOverride(short *items) {
    int count = 0;

    if (items != 0) {
        int base = g_InvCategoryBaseItemId;
        u16 *out = g_BattleCountTable;
        int end = base + 3;

        do {
            int id = items[0];
            if (id == 0) {
                break;
            }

            if ((base <= id) && (id < end)) {
                u16 value;
                int temp = id + 6;

                id = temp - base;
                temp = id + 0x200;
                *out = temp;
                value = (u16)items[1];
                asm("" : : "r"(value) : "$2");
                temp = id << 5;
                *(u16 *)((u8 *)g_InvCategoryItemTable + temp) = value;
                out++;
            } else {
                *out = id;
                out++;
            }

            count++;
            items++;
            items++;
        } while (count < 10);

        g_InvActiveListOverride = g_BattleCountTable;
        g_InvOverrideSlotLimit = count;
        Inv_RebuildSelectableMask();
    }

    g_MenuBattleCount = count;
    return count;
}
