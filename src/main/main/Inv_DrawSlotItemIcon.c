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
extern u8 g_InvGenericItemIconSlot[] __asm__("D_800A1E44");
extern u8 g_InvTypedItemIconSlots[] __asm__("D_800A1E64");
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");

#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)
#define U8(base, off) (*(u8 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define U16(base, off) (*(u16 *)((u8 *)(base) + (off)))

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

void Inv_DrawSlotItemIcon(void) {
    int type;
    int amount;
    int max_total;
    u8 *data;
    u8 *pool;

    LOOKUP_SLOT_DATA(g_AyaEquippedWeaponSlot, data);
    type = data[6];

    if (type != 0 && type < 8) {
        pool = g_InvTypedItemIconSlots;
        if (type > 4) {
            pool += (type - 5) << 5;
        }
    } else if (type >= 0x13) {
        pool = g_InvTypedItemIconSlots + ((type - 0x13) << 5);
    } else {
        pool = g_InvGenericItemIconSlot;
    }

    max_total = U8(data, 9) + S16(data, 0x12);
    if (max_total >= 1000) {
        max_total = 999;
    }

    amount = max_total - U16(data, 0xA);
    if (amount >= U16(pool, 0xA)) {
        amount = U16(pool, 0xA);
    }

    U16(pool, 0xA) -= amount;
    U16(data, 0xA) += amount;
}
