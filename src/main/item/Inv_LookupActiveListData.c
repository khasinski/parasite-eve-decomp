/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef unsigned char u8;
typedef short s16;

int g_InvSlotLimit;
int g_InvItemPtr;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];

void *Item_LookupBaseData(int index);

void *Inv_LookupActiveListData(int index) {
    int value;
    int offset;
    int final_range;
    register int saved_value asm("$5");

    if (index < 0) {
        return 0;
    }
    if (index >= g_InvSlotLimit) {
        return 0;
    }

    value = ((s16 *)g_InvItemPtr)[index];
    saved_value = value;
    if ((unsigned int)(value - 0x100) < 0x80) {
        return g_EquipItemDataTable + (value << 5);
    }

    offset = value - 1;
    if ((unsigned int)offset < 0xFF) {
        return Item_LookupBaseData(offset);
    }

    final_range = saved_value - 0x200;
    if ((unsigned int)final_range >= 9) {
        return 0;
    }
    {
        register int shifted asm("$3") = saved_value << 5;
        return g_KeyItemDataTable + shifted;
    }
}
