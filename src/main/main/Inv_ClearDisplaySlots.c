/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s16 *g_InvDisplayListPtr __asm__("D_8009D04C");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 g_InvDisplayListCount __asm__("D_8009D054");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u8 g_AyaTempItemDataSlots[] __asm__("D_800C0EAC");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

int Inv_GetAyaSlotLimit(void);
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

#define COPY_ITEM_DATA(dst_, src_)                          \
    do {                                                    \
        int copy_i_;                                        \
        for (copy_i_ = 0; copy_i_ < 0x20; copy_i_++) {      \
            (dst_)[copy_i_] = (src_)[copy_i_];              \
        }                                                   \
    } while (0)

void Inv_ClearDisplaySlots(void) {
    int index;
    int temp_index;
    u8 *data;
    u8 *temp_slot;
    u8 *temp_slot_end;

    g_InvDisplayListPtr = 0;
    g_InvDisplayListCount = 0;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    for (index = 0; index < g_InvSlotLimit; index++) {
        LOOKUP_SLOT_DATA(index, data);
        if ((unsigned int)((data != 0 ? data[6] : 0) - 1) >= 9) {
            continue;
        }

        if ((unsigned int)(g_InvItemPtr[index] - 0x100) < 0x80) {
            continue;
        }

        temp_slot = g_AyaTempItemDataSlots;
        temp_slot_end = g_AyaTempItemDataSlots + 0x1000;
        while (temp_slot < temp_slot_end && temp_slot[0] != 0) {
            temp_slot += 0x20;
        }

        if (temp_slot >= temp_slot_end) {
            temp_index = -1;
        } else {
            temp_index = (temp_slot - g_AyaTempItemDataSlots) >> 5;
        }

        if (temp_index >= 0) {
            temp_slot = g_AyaTempItemDataSlots + temp_index * 0x20;
            data = Item_LookupBaseData(g_InvItemPtr[index] - 1);
            COPY_ITEM_DATA(temp_slot, data);
            g_InvItemPtr[index] = temp_index + 0x100;
        }
    }
}
