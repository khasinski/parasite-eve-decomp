/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/inventory_slots.h"

extern unsigned char D_800C1EAC[];
extern signed char g_AyaEquippedWeaponSlot[];
extern signed char g_AyaEquippedArmorSlot[];

unsigned int g_SavedMenuMode;

unsigned int Menu_GetActiveMode(void);
void Window_SetBoundsByMode(int mode);

int Inv_FindItem(unsigned int arg0) {
    int result;
    register ItemDataRecord *limit asm("$18");
    ItemDataRecord *slot;
    ItemDataRecord *entry;
    short *item;
    short *end;
    register int value asm("$3");
    register int savedValue asm("$5");
    int index;
    ItemDataRecord *base;
    unsigned int count;

    result = arg0;
    slot = g_InvItemSlotArray;
    if (slot < g_InvItemSlotArray + 0x80) {
        while (1) {
            if (slot->itemId == result) {
                break;
            }
            slot++;
            if (slot >= g_InvItemSlotArray + 0x80) {
                break;
            }
        }

        limit = (ItemDataRecord *)D_800C1EAC;
        if (slot < limit) {
            g_InvItemPtr = (s16 *)((u8 *)limit - 0x1064);
            count = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_AyaItemSelectionBits;
            g_InvSelectionBitWords = 2;
            asm volatile("" : "=r"(slot), "=r"(limit), "=r"(count) : "0"(slot), "1"(limit), "2"(count));

            base = limit - 0x80;
            value = (slot - base) + 0x100;
            item = g_InvItemPtr;
            g_InvSlotLimit = count;
            end = item + count;

            if (item < end) {
                while (*item != value) {
                    item++;
                    if (item >= end) {
                        break;
                    }
                }
                if (item < g_InvItemPtr + g_InvSlotLimit) {
                    index = item - g_InvItemPtr;
                } else {
                    index = -1;
                }
            } else {
                index = -1;
            }
            result = index;

            if (result >= 0) {
                if (result < g_InvSlotLimit) {
                    value = g_InvItemPtr[result];
                    asm volatile("" : "=r"(value) : "0"(value));
                    savedValue = value;
                    if ((unsigned int)(value - 0x100) < 0x80) {
                        entry = &((ItemDataRecord *)g_EquipItemDataTable)[value];
                    } else if ((unsigned int)(value - 1) < 0xFF) {
                        entry = Item_LookupBaseData(value - 1);
                    } else if ((unsigned int)(savedValue - 0x200) < 9) {
                        value = savedValue << 5;
                        entry = (ItemDataRecord *)(g_KeyItemDataTable + value);
                    } else {
                        entry = 0;
                    }
                } else {
                    entry = 0;
                }

                value = 0;
                if (entry != 0) {
                    value = entry->kind;
                }

                if (value != 0) {
                    if (value < 9) goto type_less_than_9;
                    if (value == 9) goto type_eq_9;
                    result = -1;
                    goto done;
type_less_than_9:
                    g_AyaEquippedWeaponSlot[0] = result;
                    return (unsigned int)result >> 31;
type_eq_9:
                    g_AyaEquippedArmorSlot[0] = result;
                    return (unsigned int)result >> 31;
                }
                result = -1;
            }
        }
    }

done:
    return (unsigned int)result >> 31;
}


void Menu_ResetInputState(void) {
    g_SavedMenuMode = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
}
