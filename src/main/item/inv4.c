/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef unsigned char u8;

extern short *g_InvActiveItemList;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];

void *Item_LookupBaseData(unsigned int index);

int g_InvTransferItemCount;
short g_InvTransferItemList[];

u8 *Inv_LookupItemData(int arg0) {
    int value = g_InvActiveItemList[arg0];
    int savedValue = value;
    u8 *entry;

    if ((unsigned int)(value - 0x100) < 0x80) {
        entry = g_EquipItemDataTable + (value << 5);
    } else if ((unsigned int)(value - 1) < 0xFF) {
        entry = Item_LookupBaseData(value - 1);
    } else if ((unsigned int)(savedValue - 0x200) < 9) {
        int shifted = savedValue << 5;

        entry = g_KeyItemDataTable + shifted;
    } else {
        entry = 0;
    }

    return entry;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/item/inv4", Inv_TransferItemAlt2);

int Inv_GetWayneListItemByIndex(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvTransferItemCount) {
        return g_InvTransferItemList[arg0];
    }
    return 0;
}
