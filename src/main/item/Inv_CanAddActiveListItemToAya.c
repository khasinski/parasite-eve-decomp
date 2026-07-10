/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s32 g_InvActiveListCount __asm__("D_8009D078");
extern s16 g_InvActiveListItems[] __asm__("D_800A1FD4");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");

void *Item_LookupBaseData(unsigned int index);
int Inv_GetAyaSlotLimit(void);
int Inv_WriteSlotById(void *data);

int Inv_CanAddActiveListItemToAya(int index) {
    register int slot_index asm("$17");
    register void *data asm("$16");
    register int saved_item_id asm("$5");
    int item_id;
    register int result asm("$3");

    slot_index = index;
    if (slot_index < 0) {
        goto fail;
    }
    if (slot_index >= g_InvActiveListCount) {
        goto fail;
    }

    {
        register int offset asm("$2");

        offset = slot_index << 1;
        item_id = *(s16 *)((u8 *)g_InvActiveListItems + offset);
    }
    saved_item_id = item_id;
    if ((unsigned int)(item_id - 0x100) < 0x80) {
        register int shifted asm("$3");
        register u8 *base asm("$2");

        shifted = item_id << 5;
        base = g_EquipItemDataTable;
        data = shifted + base;
    } else if ((unsigned int)(item_id - 1) < 0xFF) {
        data = Item_LookupBaseData(item_id - 1);
    } else if ((unsigned int)(saved_item_id - 0x200) < 9) {
        register int shifted asm("$3");
        register u8 *base asm("$2");

        shifted = saved_item_id << 5;
        base = g_KeyItemDataTable;
        data = shifted + base;
    } else {
        data = 0;
    }

    g_InvItemPtr = (int)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = (int)g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    result = Inv_WriteSlotById(data);
    asm volatile("" : "=r"(result) : "0"(result));
    if (result == 0) {
        register int offset asm("$2");

        offset = slot_index << 1;
        *(s16 *)((u8 *)g_InvActiveListItems + offset) = 0;
    }
    return result;

fail:
    result = 1;
    asm volatile("" : "=r"(result) : "0"(result));
    return result;
}
