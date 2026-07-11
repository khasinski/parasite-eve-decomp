/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern struct { char _[16]; } g_AyaInventorySlotCount_obj __asm__("D_800C0E0C");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_obj __asm__("D_800C0E22");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_obj)
#define g_AyaInventorySlotCount (*(u8 *)&g_AyaInventorySlotCount_obj)

int Inv_GetAyaSlotLimit(void);
int Inv_GetBonusSlotCount(void);
void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_SLOT_DATA(slot_, out_)                              \
    do {                                                          \
        int item_id_;                                             \
        int slot_index_;                                          \
        slot_index_ = (slot_);                                    \
        if (slot_index_ < 0 || slot_index_ >= g_InvSlotLimit) {   \
            (out_) = 0;                                           \
        } else {                                                  \
            item_id_ = g_InvItemPtr[slot_index_];                 \
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

int Inv_GetActiveSlotCount(int *out_required_mask) {
    int index;
    int used_slots;
    int slot_limit;
    int required_mask;
    int modifier;
    int capacity;
    u8 *data;
    s16 *slot;
    s16 *end;

    required_mask = 0;
    LOOKUP_SLOT_DATA(g_AyaEquippedArmorSlot, data);
    if (data != 0 && data[0x14] > 0) {
        for (index = 0; index < data[0x14]; index++) {
            modifier = (data[0x15 + index] & 0x1F) - 8;
            if ((unsigned int)modifier < 3) {
                required_mask = 1 << modifier;
                break;
            }
        }
    }

    capacity = g_AyaInventorySlotCount + Inv_GetBonusSlotCount();
    if (capacity >= 0x33) {
        capacity = 0x32;
    }

    g_InvItemPtr = g_AyaInventoryItems;
    slot_limit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSlotLimit = slot_limit;
    g_InvSelectionBitWords = 2;

    used_slots = 0;
    slot = g_InvItemPtr;
    end = slot + slot_limit;
    while (slot < end) {
        if (*slot != 0) {
            used_slots++;
        }
        slot++;
    }

    if (out_required_mask != 0) {
        *out_required_mask = required_mask;
    }

    return capacity - used_slots >= required_mask;
}
