typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern s16 g_AyaInventorySlots[] __asm__("D_800C0EAC");
extern s16 g_InvActiveListItems[] __asm__("D_800A1D9C");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");

extern s32 g_InvActiveListCount __asm__("D_8009D040");
extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern u32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s32 D_8009D068;
extern s32 D_8009D0CC;
extern s32 D_8009D0D0;

void *Item_LookupBaseData(unsigned int index);
int Inv_GetAyaSlotLimit(void);
void Inv_BuildFilteredPackedList(int mask);

#define ITEM_SLOT_SIZE 0x20

#define RESOLVE_ITEM_DATA(out, item_id_)                                      \
    do {                                                                      \
        int _item_id = (item_id_);                                             \
        if ((u32)(_item_id - 0x100) < 0x80) {                                  \
            (out) = g_EquipItemDataTable + (_item_id * ITEM_SLOT_SIZE);        \
        } else if ((u32)(_item_id - 1) < 0xFF) {                               \
            (out) = Item_LookupBaseData(_item_id - 1);                         \
        } else if ((u32)(_item_id - 0x200) < 9) {                              \
            (out) = g_KeyItemDataTable + (_item_id * ITEM_SLOT_SIZE);          \
        } else {                                                              \
            (out) = 0;                                                         \
        }                                                                     \
    } while (0)

static int Inv_FindBonusSlot(int require_type9) {
    u8 *slot;
    u8 *end;
    int type;

    slot = (u8 *)g_AyaInventorySlots;
    end = slot + 0x1000;

    while (slot < end) {
        if (slot[0] != 0) {
            type = slot[6];
            if (require_type9) {
                if (type == 9 && (slot[5] & 0x10) != 0) {
                    return 1;
                }
            } else if (type < 9 && (slot[5] & 0x10) != 0) {
                return 1;
            }
        }
        slot += ITEM_SLOT_SIZE;
    }

    return 0;
}

int Inv_RebuildWithBonusSlots(int min_pe, int min_range) {
    int index;
    int active_slot;
    int item_id;
    int matches;
    u8 *data;
    int mask_word;
    int bit;

    g_InvItemPtr = (s32)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    Inv_BuildFilteredPackedList(D_8009D0CC != 0 ? 0x1FE : 0x200);

    for (index = 0; index < g_InvSelectionBitWords; index++) {
        g_InvSelectionBits[index] = 0;
    }

    matches = 0;
    for (index = 0; index < g_InvActiveListCount; index++) {
        active_slot = g_InvActiveListItems[index];
        data = 0;

        if (active_slot >= 0 && active_slot < g_InvSlotLimit) {
            item_id = ((s16 *)g_InvItemPtr)[active_slot];
            RESOLVE_ITEM_DATA(data, item_id);
        }

        if (data != 0 &&
                data[7] + *(s16 *)(data + 0xE) >= D_8009D0D0 &&
                data[8] + *(s16 *)(data + 0x10) >= min_pe &&
                data[9] + *(s16 *)(data + 0x12) >= min_range &&
                data[4] != 0x93 &&
                data[4] != 0x61) {
            matches++;
            mask_word = index >> 5;
            bit = index & 0x1F;
            g_InvSelectionBits[mask_word] |= 1 << bit;
        }
    }

    if (matches != 0 && Inv_FindBonusSlot(D_8009D0CC == 0)) {
        matches = -matches;
    }

    D_8009D068 = matches != 0;
    return matches;
}
