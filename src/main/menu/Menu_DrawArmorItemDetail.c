#include "common.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern s16 *D_8009D07C;
extern unsigned int *D_8009D058;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];
extern u8 D_800C20A4[];

void Draw_SetTextDimmed(int value);
void *Str_LookupTable8(unsigned int index);
void Sfx_DrawSlotRow(void *entry, void *display);

void Menu_DrawArmorItemDetail(int slot) {
    void *display;
    ItemDataRecord *entry;
    ItemDataRecord *candidate;
    int value;
    unsigned int bit;

    value = D_8009D07C[slot];
    if (value != 0) {
        bit = D_8009D058[slot >> 5] & (1 << (slot & 0x1F));
        Draw_SetTextDimmed(bit == 0);

        if ((unsigned int)(value - 0x100) < 0x80U) {
            candidate = (ItemDataRecord *)(g_EquipItemDataTable +
                                           (value << 5));
            goto resolved;
        } else if ((unsigned int)(value - 1) < 0xFFU) {
            candidate = Item_LookupBaseData(value - 1);
            goto resolved;
        } else if ((unsigned int)(value - 0x200) < 9U) {
            candidate = (ItemDataRecord *)(g_KeyItemDataTable + (value << 5));
            goto resolved;
        } else {
            candidate = 0;
        }

resolved:
        entry = candidate;

        if ((entry->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) != 0) {
            display = D_800C20A4;
            if (entry->kind == ITEM_KIND_ARMOR) {
                display = D_800C20A4 + 0x10;
            }
        } else {
            display = Str_LookupTable8(entry->itemId - 1);
        }

        Sfx_DrawSlotRow(entry, display);
    }
}
