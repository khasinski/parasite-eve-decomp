/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed short s16;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 D_8009D04C;
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 D_8009D054;
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u32 D_800A1F84[];

void *Item_LookupBaseData(unsigned int index);
int Inv_GetAyaSlotLimit(void);

static u8 *Inv_CompatLookupListData(int index) {
    int item_id;
    int offset;

    if (index < 0 || index >= g_InvSlotLimit) {
        return 0;
    }

    item_id = ((s16 *)g_InvItemPtr)[index];
    if ((unsigned int)(item_id - 0x100) < 0x80) {
        return g_EquipItemDataTable + item_id * 0x20;
    }

    offset = item_id - 1;
    if ((unsigned int)offset < 0xFF) {
        return Item_LookupBaseData(offset);
    }

    if ((unsigned int)(item_id - 0x200) < 9) {
        return g_KeyItemDataTable + item_id * 0x20;
    }

    return 0;
}

static int Inv_CompatClass(u8 type) {
    int result;

    if (type != 0 && type < 8) {
        result = type - 4;
        if (result <= 0) {
            result = 1;
        }
        return result;
    }

    if (type < 0x13) {
        return 0;
    }

    return type - 0x12;
}

static int Inv_CompatTypeIsSpecial(u8 type) {
    return (unsigned int)(type - 0x13) < 3;
}

static void Inv_ClearSelectionBits(void) {
    int i;

    for (i = 0; i < g_InvSelectionBitWords; i++) {
        g_InvSelectionBits[i] = 0;
    }
}

static void Inv_SetSelectionBit(int index) {
    g_InvSelectionBits[index >> 5] |= 1u << (index & 0x1F);
}

static int Inv_CountSelectionBits(void) {
    int i;
    int count;

    count = 0;
    for (i = 0; i < g_InvSlotLimit; i++) {
        if (g_InvSelectionBits[i >> 5] & (1u << (i & 0x1F))) {
            count++;
        }
    }

    return count;
}

static void Inv_BuildCompatibleBitsForCurrentList(int selected_index, int compat_class, int selected_is_special) {
    int i;

    Inv_ClearSelectionBits();

    for (i = 0; i < g_InvSlotLimit; i++) {
        u8 *data;
        u8 type;
        int item_class;
        int compatible;

        if (i == selected_index) {
            continue;
        }

        data = Inv_CompatLookupListData(i);
        if (data == 0) {
            continue;
        }

        type = data[6];
        item_class = Inv_CompatClass(type);

        if (selected_is_special) {
            compatible = (item_class == compat_class) || (item_class == 0 && compat_class == 0);
        } else if (Inv_CompatTypeIsSpecial(type)) {
            compatible = (item_class == compat_class) || (item_class == 0 && compat_class == 0);
        } else {
            compatible = 0;
        }

        if (compatible) {
            Inv_SetSelectionBit(i);
        }
    }
}

static void Inv_SelectAyaInventoryBits(void) {
    g_InvItemPtr = (s32)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
}

static void Inv_SelectSavedInventoryBits(void) {
    g_InvItemPtr = D_8009D04C;
    g_InvSlotLimit = D_8009D054;
    g_InvSelectionBits = D_800A1F84;
    g_InvSelectionBitWords = 4;
}

int Inv_BuildCompatibleWeaponBitset(int selected_index) {
    u8 *selected_data;
    u8 selected_type;
    int compat_class;
    int selected_is_special;
    int count;
    int initial_was_not_aya;

    selected_data = Inv_CompatLookupListData(selected_index);
    selected_type = selected_data[6];
    compat_class = Inv_CompatClass(selected_type);
    selected_is_special = Inv_CompatTypeIsSpecial(selected_type);

    Inv_BuildCompatibleBitsForCurrentList(selected_index, compat_class, selected_is_special);
    count = Inv_CountSelectionBits();

    initial_was_not_aya = ((s16 *)g_InvItemPtr != g_AyaInventoryItems);
    if (!initial_was_not_aya) {
        if (D_8009D04C == 0) {
            return count;
        }

        Inv_SelectSavedInventoryBits();
        Inv_BuildCompatibleBitsForCurrentList(-1, compat_class, selected_is_special);
        count += Inv_CountSelectionBits();
        Inv_SelectAyaInventoryBits();
        return count;
    }

    Inv_SelectAyaInventoryBits();
    Inv_BuildCompatibleBitsForCurrentList(-1, compat_class, selected_is_special);
    count += Inv_CountSelectionBits();

    if (D_8009D04C != 0) {
        Inv_SelectSavedInventoryBits();
    } else {
        Inv_SelectAyaInventoryBits();
    }

    return count;
}
