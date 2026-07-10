/* CC1_FLAGS: -G8 */
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

extern short D_800C0E48[];
extern int D_8009D050;
extern short *D_8009D058;
extern short D_8009D05C[];
extern int D_8009D064;

int Inv_GetAyaSlotLimit(void);
void Inv_RebuildSelectableMask(void);

void Inv_TransferItemAlt2(int mask) {
    register short *out asm("$17");
    register int index asm("$16");
    register int mask_reg asm("$18") = mask;
    u8 *data;
    int limit;
    int value;
    short slot;

    out = g_InvTransferItemList;
    g_InvActiveItemList = D_800C0E48;
    limit = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D050 = limit;
    D_8009D064 = 2;

    for (index = 0; index < D_8009D050; index++) {
        data = 0;
        if (index >= 0 && index < D_8009D050) {
            value = g_InvActiveItemList[index];
            if ((unsigned int)(value - 0x100) < 0x80) {
                data = g_EquipItemDataTable + (value << 5);
            } else if ((unsigned int)(value - 1) < 0xFF) {
                data = Item_LookupBaseData(value - 1);
            } else if ((unsigned int)(value - 0x200) < 9) {
                data = g_KeyItemDataTable + (value << 5);
            }
        }
        if (((mask_reg >> (data != 0 ? data[6] : 0)) & 1) != 0) {
            slot = index;
            *out++ = slot;
        }
    }

    g_InvTransferItemCount = (out - g_InvTransferItemList);
    Inv_RebuildSelectableMask();

    for (index = 0; index < D_8009D050; index++) {
        if (g_InvActiveItemList[index] == 0) {
            slot = index;
            *out++ = slot;
        }
    }

    g_InvTransferItemCount = (out - g_InvTransferItemList);
}

int Inv_GetWayneListItemByIndex(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvTransferItemCount) {
        return g_InvTransferItemList[arg0];
    }
    return 0;
}
