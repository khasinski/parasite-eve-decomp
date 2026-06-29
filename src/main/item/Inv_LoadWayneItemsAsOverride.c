/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned short u16;

extern int g_InvCategoryBaseItemId;
extern u16 g_BattleCountTable[];
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
            register int id asm("$5") = items[0];
            if (id == 0) {
                break;
            }

            if ((base <= id) && (id < end)) {
                register int index asm("$5");
                int temp = id + 6;

                index = temp - base;
                temp = index + 0x200;
                *out = temp;
                __asm__ volatile(
                    ".set push\n"
                    ".set noat\n"
                    "lhu $3, 2($4)\n"
                    "sll $2, $5, 5\n"
                    "lui $1, %%hi(g_InvCategoryItemTable)\n"
                    "addu $1, $1, $2\n"
                    "sh $3, %%lo(g_InvCategoryItemTable)($1)\n"
                    ".set pop"
                    :
                    : "r"(items), "r"(index)
                    : "$1", "$2", "$3", "memory");
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
