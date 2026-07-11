/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_obj __asm__("D_800C0E22");

#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)
#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_obj)

void *Item_LookupBaseData(unsigned int index);
int Inv_GetActiveListItemType(int index);

#define LOOKUP_SLOT_DATA(slot_, out_)                              \
    do {                                                          \
        int item_id_;                                             \
        if ((slot_) < 0 || (slot_) >= g_InvSlotLimit) {           \
            (out_) = 0;                                           \
        } else {                                                  \
            item_id_ = g_InvItemPtr[(slot_)];                     \
            if ((unsigned int)(item_id_ - 0x100) < 0x80) {        \
                (out_) = g_EquipItemDataTable + (item_id_ << 5);  \
            } else if ((unsigned int)(item_id_ - 1) < 0xFF) {     \
                (out_) = Item_LookupBaseData(item_id_ - 1);       \
            } else if ((unsigned int)(item_id_ - 0x200) < 9) {    \
                (out_) = g_KeyItemDataTable + (item_id_ << 5);    \
            } else {                                              \
                (out_) = 0;                                       \
            }                                                     \
        }                                                         \
    } while (0)

int Inv_IsSlotEquipped(int slot) {
    int equipped;
    int index;
    int matching_count;
    u8 *data;

    LOOKUP_SLOT_DATA(slot, data);
    if (data == 0) {
        return 1;
    }

    equipped = 0;
    if (!(data[5] & 0x20)) {
        if (g_InvItemPtr != g_AyaInventoryItems ||
            (g_AyaEquippedWeaponSlot != slot && g_AyaEquippedArmorSlot != slot)) {
            equipped = 1;
        }
    }

    if (data[6] != 8) {
        return equipped;
    }

    matching_count = 0;
    for (index = 0; index < g_InvSlotLimit; index++) {
        if (Inv_GetActiveListItemType(index) == 8) {
            matching_count++;
        }
    }

    if (matching_count < 2) {
        equipped = 0;
    }
    return equipped & 1;
}
