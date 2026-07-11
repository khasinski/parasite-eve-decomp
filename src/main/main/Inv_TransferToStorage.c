/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s32 g_InvActiveListCount __asm__("D_8009D040");
extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern s16 g_InvActiveListItems[] __asm__("D_800A1D9C");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

int Inv_GetAyaSlotLimit(void);
void Inv_BuildFilteredPackedList(int mask);
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

int Inv_TransferToStorage(void) {
    int count;
    int index;
    int bit;
    int slot;
    u8 *data;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    Inv_BuildFilteredPackedList(0x3FE);

    for (index = 0; index < g_InvSelectionBitWords; index++) {
        g_InvSelectionBits[index] = 0;
    }

    count = 0;
    for (index = 0; index < g_InvActiveListCount; index++) {
        if (index >= 0 && index < g_InvActiveListCount) {
            slot = g_InvActiveListItems[index];
        } else {
            slot = 0;
        }

        LOOKUP_SLOT_DATA(slot, data);
        if (data[0x14] < data[1]) {
            count++;
            bit = index & 0x1F;
            g_InvSelectionBits[index >> 5] |= 1 << bit;
        }
    }

    return count;
}
