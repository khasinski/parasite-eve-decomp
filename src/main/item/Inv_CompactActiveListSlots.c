/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern u8 g_AyaInventorySlotCount[0x40] __asm__("D_800C0E0C");
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_obj __asm__("D_800C0E22");
#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)
#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_obj)

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 D_8009D06C;

int Inv_GetBonusSlotCount(void);

void Inv_CompactActiveListSlots(void) {
    int empty_index;
    int compact_from;
    int slot_limit;
    s16 *slot;
    s16 *end;

    while (D_8009D06C > 0) {
        slot = g_InvItemPtr;
        end = g_InvItemPtr + g_InvSlotLimit;

        while (slot < end && *slot != 0) {
            slot++;
        }

        if (slot < g_InvItemPtr + g_InvSlotLimit) {
            empty_index = slot - g_InvItemPtr;
        } else {
            empty_index = -1;
        }

        if (empty_index >= 0) {
            if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() < 0x33) {
                slot_limit = g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
            } else {
                slot_limit = 0x32;
            }

            compact_from = slot_limit - D_8009D06C;
            g_InvItemPtr[empty_index] = (u16)g_InvItemPtr[compact_from];
            g_InvItemPtr[compact_from] = 0;

            if (g_AyaEquippedWeaponSlot == compact_from) {
                g_AyaEquippedWeaponSlot = empty_index;
            } else if (g_AyaEquippedArmorSlot == compact_from) {
                g_AyaEquippedArmorSlot = empty_index;
            }
        }

        D_8009D06C--;
    }
}
