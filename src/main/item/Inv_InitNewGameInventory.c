/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/aya.h"
#include "pe1/menu_inventory.h"

static inline void SelectAya(void) {
    D_8009D048 = D_800C0E00.inventory_items;
    D_8009D050 = Inv_GetAyaSlotLimit();
    D_8009D058 = D_8009D05C;
    D_8009D064 = 2;
}

/* Carry a tagged record forward, removing its first storage-list reference. */
static inline int TakeTagged(int armor) {
    ItemDataRecord *p;
    int id = 0;
    for (p = D_800C0E20.equipment; p < D_800C0E20.equipment + 128; p++) {
        if (p->pad_00[0] && (armor ? p->kind == 9 : p->kind != 9) && (p->flags & 16)) break;
    }
    if (p < D_800C0E20.equipment + 128) {
        s16 *slot;
        id = (p - D_800C0E20.equipment) + 256;
        for (slot = D_800C0E20.specialStorage;
             slot < D_800C0E20.specialStorage + 82; slot++)
            if (*slot == id) break;
        if (slot < D_800C0E20.specialStorage + 82) *slot = 0;
    }
    return id;
}

void Inv_InitNewGameInventory(void) {
    int i, capacity;
    AyaLevelStats *stats;
    u16 *growth = D_800C0E28;
    for (i = 0; i < 7; i++) *growth++ = *(u16 *)Stat_GetGrowthTable(i);
    stats = Aya_LookupLevelStats(0);
    D_800C0E00.max_hp = D_800C0E00.current_hp = stats->hp;
    D_800C0E00.inventory_slot_count = stats->inventoryCapacity;
    D_800C0E00.parasite_spell_flags = 1;
    D_800A1E64[0].ammo = D_800A1E64[1].ammo = D_800A1E64[2].ammo = 0;
    SelectAya();
    capacity = stats->inventoryCapacity;
    if (capacity > 50) capacity = 50;
    D_800C0E00.inventory_slot_count = capacity;
    if (D_8009D048 == D_800C0E00.inventory_items)
        D_8009D050 = Inv_GetAyaSlotLimit();
    SelectAya();
    for (i = 49; i >= 0; i--) D_8009D048[i] = 0;
    Inv_CheckSlotUsable(0x44);
    Inv_CheckSlotUsable(0x96);
    Inv_CheckSlotUsable(0x3F);
    Inv_CheckSlotUsable(1);
    Inv_CheckSlotUsable(6);
    i = TakeTagged(0);
    if (i) Inv_CheckSlotUsable(i);
    i = TakeTagged(1);
    if (i) Inv_CheckSlotUsable(i);
    D_800C0E20.tracked[2] = 1;
    D_800C0E20.tracked[0] = 0;
    D_800C0E00.menu_clamp_value = 61;
    Menu_ClampRange(61);
    D_800C0E00.total_exp = 0;
    D_800C0E00.level = 0;
    Menu_SaveBgInitFade();
}
