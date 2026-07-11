/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern s32 g_MenuBattleEquipMode __asm__("D_8009D028");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");

void *BattleCmd_AllocSlot(void);
void BattleCmd_CommitAndSyncAmmo(int item_id);
void Inv_RemoveActiveListItem(int slot);
void Inv_RebuildSelectableMask(void);
void Inv_SetActiveList(int mode, void *selected);
void Menu_OpenSkillSelectionView(void);
void Menu_StepInventoryRoot(int flags, int selected, int arg2);
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

void Inv_BuildEquipSlotDisplay(int slot) {
    int selected_item;
    u8 *data;
    u32 *cmd;

    LOOKUP_SLOT_DATA(slot, data);

    if (g_MenuBattleEquipMode != 0) {
        cmd = BattleCmd_AllocSlot();
        selected_item = g_InvItemPtr[slot];
        cmd[0] = 0;
        cmd[2] = slot;
        cmd[1] = selected_item;
        Inv_SetActiveList(0, &selected_item);
        Inv_RemoveActiveListItem(slot);
        return;
    }

    switch (data[0xE]) {
    case 1:
        BattleCmd_CommitAndSyncAmmo(g_InvItemPtr[slot]);
        Inv_RemoveActiveListItem(slot);
        Inv_RebuildSelectableMask();
        break;
    case 2:
        Menu_OpenSkillSelectionView();
        break;
    case 4:
    case 5:
    case 6:
        Menu_StepInventoryRoot(0xFE, slot, -1);
        break;
    case 0xC:
    case 0xD:
    case 0xE:
        Menu_StepInventoryRoot(0x200, slot, -1);
        break;
    }
}
