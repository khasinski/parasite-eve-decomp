/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value;
    register int saved asm("$5");
    if (index < 0 || index >= D_8009D050) return 0;
    value = D_8009D048[index];
    saved = value;
    if ((unsigned)(value - 0x100) < 0x80)
        return &D_800C0E20.equipment[value - 0x100];
    if ((unsigned)(value - 1) < 0xFF)
        return Item_LookupBaseData(value - 1);
    if ((unsigned)(saved - 0x200) < 9) {
        register int shifted asm("$3") = saved << 5;
        return (ItemDataRecord *)(D_8009DE64 + shifted);
    }
    return 0;
}
static inline int GetFilteredSlot(int index) {
    int result;
    if (index >= 0 && index < D_8009D040)
        result = D_800A1D9C[index];
    else result = 0;
    return result;
}
void Menu_OpenItemUsePanelAtIndex(int index) {
    BattleCmdEntry *command;
    int slot;
    command = BattleCmd_AllocSlot();
    command->header.word = 2;
    command->payload.equip_restore.item_data = LookupTrackedItem(D_800C0E20.tracked[0]);
    slot = GetFilteredSlot(index);
    D_800C0E20.tracked[0] = GetFilteredSlot(index);
    Inv_SetActiveList(2, &slot);
}
