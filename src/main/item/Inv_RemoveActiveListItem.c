/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#include "../../../tools/m2c/m2c_macros.h"

M2C_UNK Inv_SetActiveList();
M2C_UNK Inv_CompactActiveListSlots();
M2C_UNK Inv_CheckFreeSlotCapacity();
M2C_UNK Inv_GetActiveSlotCount();

extern s32 g_InvItemPtr;
extern struct { char _[16]; } D_800C0E22_l0_o __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_l1_o __asm__("g_AyaEquippedArmorSlot");
extern struct { char _[16]; } D_800C0E22_s0_o __asm__("g_AyaEquippedArmorSlot");
#define D_800C0E22_l0 (*(s8 *)&D_800C0E22_l0_o)
#define D_800C0E22_l1 (*(s8 *)&D_800C0E22_l1_o)
#define D_800C0E22_s0 (*(s8 *)&D_800C0E22_s0_o)

extern struct { char _[16]; } D_800C0EAC_o __asm__("g_InvItemSlotArray");
#define g_InvItemSlotArray (*(s8 *)&D_800C0EAC_o)

extern struct { char _[16]; } g_AyaInventoryItems_o __asm__("g_AyaInventoryItems");
#define g_AyaInventoryItems (*(M2C_UNK *)&g_AyaInventoryItems_o)

s32 Inv_RemoveActiveListItem(s32 arg0) {
    s32 sp10;
    s16 *slot;
    s32 selected;
    s32 removed;
    s32 offset;
    s32 activeList;

    selected = arg0;
    if ((g_InvItemPtr == &g_AyaInventoryItems) && (D_800C0E22_l0 == selected)) {
        Inv_GetActiveSlotCount(&sp10);
    }

    activeList = g_InvItemPtr;
    offset = selected << 1;
    slot = (s16 *)(offset + activeList);
    activeList = *slot;
    *slot = 0;
    removed = activeList;

    if (activeList >= 0x100) {
        *(&g_InvItemSlotArray + ((removed - 0x100) << 5)) = 0;
    }

    if ((g_InvItemPtr == &g_AyaInventoryItems) && (D_800C0E22_l1 == selected)) {
        D_800C0E22_s0 = -1;
        Inv_CheckFreeSlotCapacity(sp10);
        Inv_CompactActiveListSlots();
        Inv_SetActiveList(3, 0);
    }

    return removed;
}
