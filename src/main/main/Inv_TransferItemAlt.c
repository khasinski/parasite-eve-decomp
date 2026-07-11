/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s32 g_BattleItemCount __asm__("D_8009D078");
extern s16 g_BattleCountTable[] __asm__("D_800A1FD4");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");

int Inv_GetAyaSlotLimit(void);
int Inv_CanAddActiveListItemToAya(int index);
void Inv_RebuildSelectableMask(void);
void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_ITEM_DATA(item_id_, out_)                            \
    do {                                                          \
        int item_id__ = (item_id_);                               \
        if ((unsigned int)(item_id__ - 0x100) < 0x80) {           \
            (out_) = g_EquipItemDataTable + (item_id__ << 5);     \
        } else if ((unsigned int)(item_id__ - 1) < 0xFF) {        \
            (out_) = Item_LookupBaseData(item_id__ - 1);          \
        } else if ((unsigned int)(item_id__ - 0x200) < 9) {       \
            (out_) = g_KeyItemDataTable + (item_id__ << 5);       \
        } else {                                                  \
            (out_) = 0;                                           \
        }                                                         \
    } while (0)

void Inv_TransferItemAlt(void) {
    int index;
    int empty_slot;
    int item_id;
    u8 *data;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    for (index = 0; index < g_BattleItemCount; index++) {
        item_id = g_BattleCountTable[index];
        LOOKUP_ITEM_DATA(item_id, data);

        if (data[6] >= 0x10) {
            LOOKUP_ITEM_DATA(item_id, data);
            if (data[6] < 0x13) {
                if (!Inv_CanAddActiveListItemToAya(index)) {
                    g_BattleCountTable[index] = 0;
                }
                continue;
            }
        }

        if (item_id < 0x200) {
            for (empty_slot = 0; empty_slot < g_InvSlotLimit; empty_slot++) {
                if (g_InvItemPtr[empty_slot] == 0) {
                    break;
                }
            }
            if (empty_slot >= g_InvSlotLimit) {
                empty_slot = -1;
            }
            if (empty_slot >= 0) {
                g_InvItemPtr[empty_slot] = item_id;
                g_BattleCountTable[index] = 0;
            }
        }
    }

    Inv_RebuildSelectableMask();
}
