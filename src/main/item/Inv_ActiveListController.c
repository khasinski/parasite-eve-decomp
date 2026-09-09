#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "m2c_macros.h"

void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_GridCellIndex(void *node);
M2C_UNK Inv_SetActiveList();
M2C_UNK Inv_CompactActiveListSlots();
M2C_UNK Inv_CheckFreeSlotCapacity();
M2C_UNK Inv_GetActiveSlotCount();
void Inv_RebuildSelectableMask(void);

extern s16 *g_InvItemPtr;
extern s8 g_AyaEquippedWeaponSlot[];
extern s8 g_AyaEquippedArmorSlot[];
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
extern int g_InvCategoryBaseItemId;
extern u16 g_BattleCountTable[];
extern u16 g_InvCategoryItemTable[][16];
extern u16 *g_InvActiveListOverride;
extern int g_InvOverrideSlotLimit;
extern int g_MenuBattleCount;

void Inv_DropCurrentSelectionItem(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(2, 1);
    int index;

    if (node != 0) {
        index = MenuWidget_GridCellIndex(node);
        if (index >= 0) {
            Inv_RemoveActiveListItem(index);
        }
    }
}

int Inv_SwapSlots(int unused, int from, int unused2, int to) {
    g_InvItemPtr[from] ^= g_InvItemPtr[to];
    g_InvItemPtr[to] ^= g_InvItemPtr[from];
    g_InvItemPtr[from] ^= g_InvItemPtr[to];

    if (g_AyaEquippedWeaponSlot[0] == from) {
        g_AyaEquippedWeaponSlot[0] = to;
    } else if (g_AyaEquippedWeaponSlot[0] == to) {
        g_AyaEquippedWeaponSlot[0] = from;
    }

    if (g_AyaEquippedArmorSlot[0] == from) {
        g_AyaEquippedArmorSlot[0] = to;
    } else if (g_AyaEquippedArmorSlot[0] == to) {
        g_AyaEquippedArmorSlot[0] = from;
    }

    Inv_RebuildSelectableMask();
    return 1;
}


int Inv_ClearActiveListSlot(int arg0) {
    int value;

    value = g_InvItemPtr[arg0];
    g_InvItemPtr[arg0] = 0;
    return value;
}


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


int Inv_LoadWayneItemsAsOverride(short *items) {
    int count = 0;

    if (items != 0) {
        int base = g_InvCategoryBaseItemId;
        u16 *out = g_BattleCountTable;
        int end = base + 3;

        do {
            int id = items[0];
            if (id == 0) {
                break;
            }

            if ((base <= id) && (id < end)) {
                u16 value;
                int temp = id + 6;

                id = temp - base;
                temp = id + 0x200;
                *out = temp;
                value = (u16)items[1];
                asm("" : : "r"(value) : "$2");
                temp = id << 5;
                *(u16 *)((u8 *)g_InvCategoryItemTable + temp) = value;
                out++;
            } else {
                *out = id;
                out++;
            }

            count++;
            items++;
            items++;
        } while (count < 10);

        g_InvActiveListOverride = g_BattleCountTable;
        g_InvOverrideSlotLimit = count;
        Inv_RebuildSelectableMask();
    }

    g_MenuBattleCount = count;
    return count;
}
