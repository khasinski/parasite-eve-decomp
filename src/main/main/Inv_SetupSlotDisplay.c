/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern u32 D_800A1F84[];
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s16 *D_8009D04C;
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 D_8009D054;
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");

int Inv_GetAyaSlotLimit(void);
void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_ITEM_DATA(index_, out_)                         \
    do {                                                       \
        int item_id_;                                          \
        if ((index_) < 0 || (index_) >= g_InvSlotLimit) {      \
            (out_) = 0;                                        \
        } else {                                               \
            item_id_ = g_InvItemPtr[(index_)];                 \
            if ((unsigned int)(item_id_ - 0x100) < 0x80) {     \
                (out_) = g_EquipItemDataTable + (item_id_ << 5); \
            } else if ((unsigned int)(item_id_ - 1) < 0xFF) {  \
                (out_) = Item_LookupBaseData(item_id_ - 1);    \
            } else if ((unsigned int)(item_id_ - 0x200) < 9) { \
                (out_) = g_KeyItemDataTable + (item_id_ << 5); \
            } else {                                           \
                (out_) = 0;                                    \
            }                                                  \
        }                                                      \
    } while (0)

#define COUNT_MATCHING_DISPLAY_ITEMS(mask_, count_)            \
    do {                                                       \
        int index_;                                            \
        u8 *data_;                                             \
        for (index_ = 0; index_ < g_InvSlotLimit; index_++) {  \
            LOOKUP_ITEM_DATA(index_, data_);                   \
            if (data_ != 0 && (((mask_) >> data_[6]) & 1)) {   \
                (count_) += data_[0x14] != 0;                  \
            }                                                  \
        }                                                      \
    } while (0)

int Inv_SetupSlotDisplay(int mask) {
    int count;
    int was_not_aya_list;

    count = 0;
    was_not_aya_list = g_InvItemPtr != g_AyaInventoryItems;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    COUNT_MATCHING_DISPLAY_ITEMS(mask, count);

    if (D_8009D04C != 0 && !(mask & 0x100)) {
        g_InvItemPtr = D_8009D04C;
        g_InvSlotLimit = D_8009D054;
        g_InvSelectionBits = D_800A1F84;
        g_InvSelectionBitWords = 4;

        COUNT_MATCHING_DISPLAY_ITEMS(mask, count);
    }

    if (was_not_aya_list && D_8009D04C != 0) {
        g_InvItemPtr = D_8009D04C;
        g_InvSlotLimit = D_8009D054;
        g_InvSelectionBits = D_800A1F84;
        g_InvSelectionBitWords = 4;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
    }

    return count >= 2;
}
