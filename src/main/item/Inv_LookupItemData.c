/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;

extern short *g_InvActiveItemList;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];

u8 *Item_LookupBaseData(int index);

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
