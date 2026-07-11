/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s16 *g_InvActiveListOverride __asm__("D_8009D04C");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 g_InvOverrideSlotLimit __asm__("D_8009D054");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s32 g_InvActiveListCursor0 __asm__("D_8009D090");
extern s32 g_InvActiveListCursor1 __asm__("D_8009D094");
extern s32 g_InvActiveListOverrideEnabled[] __asm__("D_8009D098");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern s8 g_AyaEquippedWeaponSlot[] __asm__("D_800C0E20");
extern s8 g_AyaEquippedArmorSlot[] __asm__("D_800C0E22");
extern u8 g_InvItemSlotArray[] __asm__("D_800C0EAC");
extern s16 g_WayneStorageItems[] __asm__("D_800C1EB8");
extern s16 g_BattleCountTable[] __asm__("D_800A1FD4");
extern u32 D_800A1F84[];
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 D_800A1E44[];
extern u8 D_800A1E64[];

void *Item_LookupBaseData(unsigned int index);
int Inv_GetAyaSlotLimit(void);
void Inv_RebuildSelectableMask(void);
void Menu_CreateNotificationDialog(int id, int arg1);
void Inv_GetActiveSlotCount(int *out);
void Inv_CheckFreeSlotCapacity(int count);
void Inv_CompactActiveListSlots(void);
void Inv_SetActiveList(int mode, void *selected);

static int Inv_Cap999(int value) {
    if (value > 0x3E7) {
        return 0x3E7;
    }
    if (value < 0) {
        return 0;
    }
    return value;
}

static void Inv_SelectList(int list) {
    if ((unsigned int)list < 2 && g_InvActiveListOverrideEnabled[list] != 0 && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
        g_InvSelectionBits = D_800A1F84;
        g_InvSelectionBitWords = 4;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
    }
}

static int Inv_ListCursor(int list) {
    if (list == 0) {
        return g_InvActiveListCursor0;
    }
    if (list == 1) {
        return g_InvActiveListCursor1;
    }
    return -1;
}

static u8 *Inv_ResolveItemId(int item_id) {
    if ((unsigned int)(item_id - 0x100) < 0x80) {
        return g_EquipItemDataTable + item_id * 0x20;
    }
    if ((unsigned int)(item_id - 1) < 0xFF) {
        return Item_LookupBaseData(item_id - 1);
    }
    if ((unsigned int)(item_id - 0x200) < 9) {
        return g_KeyItemDataTable + item_id * 0x20;
    }
    return 0;
}

static u8 *Inv_ResolveActiveSlot(int index) {
    if (index < 0 || index >= g_InvSlotLimit) {
        return 0;
    }
    return Inv_ResolveItemId(g_InvItemPtr[index]);
}

static u8 *Inv_ModifierTableForType(int type) {
    if (type != 0 && type < 8) {
        type -= 4;
        if (type > 0) {
            return D_800A1E64 + type * 0x20;
        }
        return D_800A1E64;
    }
    if (type >= 0x13) {
        return D_800A1E64 + (type - 0x13) * 0x20;
    }
    return D_800A1E44;
}

static int Inv_FindEmptyModifierSlot(u8 *item, int prefer_group, int group_value) {
    int i;
    int count;

    if (item == 0) {
        return -1;
    }

    count = item[0x14];
    if (prefer_group) {
        for (i = 0; i < count; i++) {
            if ((item[0x15 + i] & 0xE0) == group_value) {
                return i;
            }
        }
    }

    for (i = 0; i < count; i++) {
        if ((item[0x15 + i] & 0x1F) == 0) {
            return i;
        }
    }

    return -1;
}

static void Inv_MoveModifier(u8 *from, u8 *to, int modifier_index) {
    int value;
    int slot;

    if (from == 0 || to == 0 || modifier_index < 0) {
        return;
    }

    value = from[0x15 + modifier_index];
    if (value == 0) {
        return;
    }

    slot = Inv_FindEmptyModifierSlot(to, (value & 0xE0) != 0, value & 0xE0);
    if (slot >= 0) {
        to[0x15 + slot] = value;
        from[0x15 + modifier_index] = 0;
    } else {
        Menu_CreateNotificationDialog(7, 0);
    }
}

static void Inv_MergeStats(u8 *from, u8 *to) {
    int type;
    u8 *table;

    if (from == 0 || to == 0) {
        return;
    }

    *(s16 *)(to + 0x0E) = Inv_Cap999(*(s16 *)(to + 0x0E) + *(s16 *)(from + 0x0E));
    *(s16 *)(to + 0x10) = Inv_Cap999(*(s16 *)(to + 0x10) + *(s16 *)(from + 0x10));
    *(s16 *)(to + 0x12) = Inv_Cap999(*(s16 *)(to + 0x12) + *(s16 *)(from + 0x12));
    *(s16 *)(from + 0x0E) = 0;
    *(s16 *)(from + 0x10) = 0;
    *(s16 *)(from + 0x12) = 0;

    if (from[6] == 9) {
        return;
    }

    if (from[9] < *(u16 *)(from + 0x0A)) {
        int removable;
        int room;

        removable = *(u16 *)(from + 0x0A) - from[9];
        room = Inv_Cap999(to[9] + *(s16 *)(to + 0x12)) - *(u16 *)(to + 0x0A);
        if (room > removable) {
            room = removable;
        }
        if (room < 0) {
            room = 0;
        }

        *(u16 *)(to + 0x0A) += room;
        type = from[6];
        table = Inv_ModifierTableForType(type);
        *(u16 *)(table + 0x0A) = Inv_Cap999(*(u16 *)(table + 0x0A) + (*(u16 *)(from + 0x0A) - from[9] - room));
        *(u16 *)(from + 0x0A) = from[9];
    }
}

static void Inv_ClearSlotIndex(int index) {
    int item_id;

    if (index < 0 || index >= g_InvSlotLimit) {
        return;
    }

    item_id = g_InvItemPtr[index];
    g_InvItemPtr[index] = 0;
    if (item_id >= 0x100) {
        g_InvItemSlotArray[(item_id - 0x100) * 0x20] = 0;
    }
}

static void Inv_PostRemoveAyaSlot(int removed_index, int saved_count) {
    if (g_InvItemPtr == g_AyaInventoryItems && g_AyaEquippedArmorSlot[0] == removed_index) {
        g_AyaEquippedArmorSlot[0] = -1;
        Inv_CheckFreeSlotCapacity(saved_count);
        Inv_CompactActiveListSlots();
        Inv_SetActiveList(3, 0);
    }
}

static int Inv_FindFirstTypeInAya(int type) {
    int i;

    Inv_SelectList(2);
    for (i = 0; i < g_InvSlotLimit; i++) {
        u8 *data = Inv_ResolveActiveSlot(i);
        if (data != 0 && data[6] == type) {
            return i;
        }
    }
    return -1;
}

static int Inv_FindFirstStorageType(int type) {
    int i;

    for (i = 0; i < 0x64; i++) {
        if (g_WayneStorageItems[i] != 0) {
            u8 *data = Item_LookupBaseData(g_WayneStorageItems[i] - 1);
            if (data != 0 && data[6] == type) {
                g_WayneStorageItems[i] = 0;
                return i;
            }
        }
    }
    return -1;
}

void Inv_TransferItemBetweenLists(int list, int modifier_index, int remove_flags, int amount) {
    int source_cursor;
    int dest_cursor;
    int saved_count;
    int dest_list;
    u8 *source;
    u8 *dest;

    saved_count = 0;

    Inv_SelectList(list);
    source_cursor = Inv_ListCursor(list);
    source = Inv_ResolveActiveSlot(source_cursor);

    dest_list = list == 0 ? 1 : 0;
    Inv_SelectList(dest_list);
    dest_cursor = Inv_ListCursor(dest_list);
    dest = Inv_ResolveActiveSlot(dest_cursor);

    if (modifier_index >= 0) {
        Inv_MoveModifier(source, dest, modifier_index);
    } else {
        Inv_MergeStats(source, dest);
    }

    if (amount < 0x3E7) {
        int type;

        if ((remove_flags & 1) != 0) {
            type = 0xC;
        } else {
            type = 0xD;
        }

        if ((remove_flags & 2) != 0) {
            Inv_FindFirstStorageType(type);
        } else {
            int slot = Inv_FindFirstTypeInAya(type);
            if (slot >= 0) {
                if (g_InvItemPtr == g_AyaInventoryItems && g_AyaEquippedArmorSlot[0] == slot) {
                    Inv_GetActiveSlotCount(&saved_count);
                }
                Inv_ClearSlotIndex(slot);
                Inv_PostRemoveAyaSlot(slot, saved_count);
            }
        }
    }

    if ((remove_flags & 1) != 0) {
        return;
    }

    Inv_SelectList(list);
    source_cursor = Inv_ListCursor(list);
    if (source_cursor >= 0) {
        if (g_InvItemPtr == g_AyaInventoryItems && g_AyaEquippedArmorSlot[0] == source_cursor) {
            Inv_GetActiveSlotCount(&saved_count);
        }
        Inv_ClearSlotIndex(source_cursor);
        Inv_PostRemoveAyaSlot(source_cursor, saved_count);
    }

    if (g_InvItemPtr == g_AyaInventoryItems && g_AyaEquippedWeaponSlot[0] == source_cursor) {
        int i;
        int weapon_slot;

        weapon_slot = -1;
        for (i = 0; i < g_InvSlotLimit; i++) {
            u8 *data = Inv_ResolveActiveSlot(i);
            if (data != 0 && data[6] == 8) {
                weapon_slot = i;
                break;
            }
        }
        g_AyaEquippedWeaponSlot[0] = weapon_slot;
        Inv_SetActiveList(2, 0);
    } else if (g_InvItemPtr == g_AyaInventoryItems && g_AyaEquippedArmorSlot[0] == source_cursor) {
        int i;
        int armor_slot;

        armor_slot = -1;
        for (i = 0; i < g_InvSlotLimit; i++) {
            u8 *data = Inv_ResolveActiveSlot(i);
            if (data != 0 && data[6] == 9) {
                armor_slot = i;
                break;
            }
        }
        g_AyaEquippedArmorSlot[0] = armor_slot;
        Inv_SetActiveList(3, 0);
    }
}
