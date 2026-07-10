typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern s16 g_InvActiveListItems[] __asm__("D_800A1E00");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");

extern s32 g_InvActiveListCount __asm__("D_8009D044");
extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s16 *D_8009D07C;

extern struct { char _[16]; } g_AyaEquippedWeaponSlot_o __asm__("g_AyaEquippedWeaponSlot");
extern struct { char _[16]; } g_AyaEquippedArmorSlot_o __asm__("g_AyaEquippedArmorSlot");
#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_o)
#define g_AyaEquippedArmorSlot (*(s8 *)&g_AyaEquippedArmorSlot_o)

void *Item_LookupBaseData(unsigned int index);
int Inv_GetAyaSlotLimit(void);
void Inv_RebuildSelectableMask(void);
int Inv_IsSlotEquipped(int slot);
void Menu_SetSwapReturnFlag(void);

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

static int Inv_MapActiveSlot(int index) {
    if (index >= 0 && index < g_InvActiveListCount) {
        return g_InvActiveListItems[index];
    }
    return 0;
}

static int Inv_CanSwapSlotItem(int item_id) {
    u8 *data;
    int type;

    RESOLVE_ITEM_DATA(data, item_id);
    type = data[6];
    return type < 0x13 || type >= 0x16;
}

static void Inv_SwapShorts(s16 *lhs, s16 *rhs) {
    s16 tmp;

    tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

int Inv_RebuildWithSlotLimit(int mode, int source_index, int other_mode, int target_index) {
    int source_slot;
    int target_slot;
    int ok;

    g_InvItemPtr = (s32)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    if (mode == 0x34 && other_mode == mode) {
        Inv_SwapShorts(&D_8009D07C[source_index], &D_8009D07C[target_index]);
        Inv_RebuildSelectableMask();
        return 1;
    }

    if (mode == 0x33 && other_mode == mode) {
        source_slot = Inv_MapActiveSlot(source_index);
        target_slot = Inv_MapActiveSlot(target_index);
        Inv_SwapShorts(&((s16 *)g_InvItemPtr)[source_slot], &((s16 *)g_InvItemPtr)[target_slot]);

        if (g_AyaEquippedWeaponSlot == source_slot) {
            g_AyaEquippedWeaponSlot = target_slot;
        } else if (g_AyaEquippedWeaponSlot == target_slot) {
            g_AyaEquippedWeaponSlot = source_slot;
        }

        if (g_AyaEquippedArmorSlot == source_slot) {
            g_AyaEquippedArmorSlot = target_slot;
        } else if (g_AyaEquippedArmorSlot == target_slot) {
            g_AyaEquippedArmorSlot = source_slot;
        }

        Inv_RebuildSelectableMask();
        return 1;
    }

    if (mode == 0x34) {
        target_slot = Inv_MapActiveSlot(target_index);
        if (Inv_IsSlotEquipped(target_slot) == 0) {
            return 0;
        }
        if (!Inv_CanSwapSlotItem(D_8009D07C[source_index])) {
            return 0;
        }

        Inv_SwapShorts(&D_8009D07C[source_index], &((s16 *)g_InvItemPtr)[target_slot]);
        Menu_SetSwapReturnFlag();
        Inv_RebuildSelectableMask();
        return 1;
    }

    source_slot = Inv_MapActiveSlot(source_index);
    if (Inv_IsSlotEquipped(source_slot) == 0) {
        return 0;
    }
    if (!Inv_CanSwapSlotItem(D_8009D07C[target_index])) {
        return 0;
    }

    Inv_SwapShorts(&((s16 *)g_InvItemPtr)[source_slot], &D_8009D07C[target_index]);
    Menu_SetSwapReturnFlag();
    Inv_RebuildSelectableMask();
    ok = 1;
    return ok;
}
