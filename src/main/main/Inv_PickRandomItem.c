/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;

extern s32 D_8009D038;
extern s32 g_InvActiveListCount __asm__("D_8009D040");
extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 D_8009D068;
extern u8 D_800A1B70[];
extern u8 D_800A1B90[];
extern u8 D_800A1D79[];
extern s16 g_InvActiveListItems[] __asm__("D_800A1D9C");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_obj __asm__("D_800C0E22");

#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)
#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_obj)

void *Item_LookupBaseData(unsigned int index);

#define NEXT_RANDOM_BYTE(out_)                    \
    do {                                          \
        int i;                                    \
        D_8009D038++;                            \
        if (D_8009D038 >= 0x209) {               \
            for (i = 0; i < 0x20; i++) {         \
                D_800A1B90[i] ^= D_800A1D79[i];  \
            }                                     \
            for (i = 0x20; i < 0x209; i++) {     \
                D_800A1B90[i] ^= D_800A1B70[i];  \
            }                                     \
            D_8009D038 = 0;                      \
        }                                         \
        (out_) = D_800A1B90[D_8009D038];         \
    } while (0)

int Inv_PickRandomItem(void) {
    s16 *out;
    int index;
    int count;
    int random_byte;
    int item_id;
    s16 picked_slot;
    u8 *data;

    out = g_InvActiveListItems;
    for (index = 0; index < g_InvSlotLimit; index++) {
        item_id = g_InvItemPtr[index];
        if ((unsigned int)(item_id - 0x100) < 0x80) {
            data = g_EquipItemDataTable + (item_id << 5);
        } else if ((unsigned int)(item_id - 1) < 0xFF) {
            data = Item_LookupBaseData(item_id - 1);
        } else if ((unsigned int)(item_id - 0x200) < 9) {
            data = g_KeyItemDataTable + (item_id << 5);
        } else {
            data = 0;
        }

        if (data != 0 && (data[5] & 0x80)) {
            if (g_InvItemPtr != g_AyaInventoryItems ||
                    (g_AyaEquippedWeaponSlot != index &&
                     g_AyaEquippedArmorSlot != index)) {
                *out++ = index;
            }
        }
    }

    count = out - g_InvActiveListItems;
    D_8009D068 = 0;
    g_InvActiveListCount = count;
    if (count == 0) {
        return 0;
    }

    NEXT_RANDOM_BYTE(random_byte);
    picked_slot = g_InvActiveListItems[(count * random_byte) >> 8];
    return g_InvItemPtr[picked_slot];
}
