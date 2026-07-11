/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_obj __asm__("D_800C0E22");

#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_obj)

void *Aya_GetParasiteSpellUnlockTable(void);
int BattleCmd_GetRemainingAmmo(int *out);
void *Item_LookupBaseData(unsigned int index);

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

int Inv_GetSlotHighlightState(int type, int fallback) {
    int index;
    int ammo_max;
    u8 *data;
    u16 *unlock_table;
    int value;

    if (type == 6 || type == 0x13) {
        return fallback;
    }

    if (type == 5) {
        BattleCmd_GetRemainingAmmo(&ammo_max);
        return ammo_max / 3;
    }

    unlock_table = Aya_GetParasiteSpellUnlockTable();
    value = unlock_table[(type * 2) + 1];

    LOOKUP_SLOT_DATA(g_AyaEquippedArmorSlot, data);
    if (data == 0) {
        return value;
    }

    for (index = 0; index < data[0x14]; index++) {
        if (data[index + 0x15] == 0xE) {
            return (value * 2) / 3;
        }
    }

    return value;
}
