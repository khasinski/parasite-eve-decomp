/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s32 g_InvActiveListCount __asm__("D_8009D040");
extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u8 g_KeyItemDataTable[] __asm__("D_8009DE64");
extern u8 g_EquipItemDataTable[] __asm__("D_800BEEAC");
extern s16 g_InvActiveListItems[] __asm__("D_800A1D9C");
extern struct { char _[16]; } g_AyaEquippedWeaponSlot_obj __asm__("D_800C0E20");

#define g_AyaEquippedWeaponSlot (*(s8 *)&g_AyaEquippedWeaponSlot_obj)

void *BattleCmd_AllocSlot(void);
void Inv_SetActiveList(int mode, void *selected);
void *Item_LookupBaseData(unsigned int index);

#define LOOKUP_SLOT_DATA(slot_, out_)                              \
    do {                                                          \
        int item_id_;                                             \
        int slot_index_;                                          \
        slot_index_ = (slot_);                                    \
        if (slot_index_ < 0 || slot_index_ >= g_InvSlotLimit) {   \
            (out_) = 0;                                           \
        } else {                                                  \
            item_id_ = g_InvItemPtr[slot_index_];                 \
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

void Menu_OpenItemUsePanelAtIndex(int index) {
    int selected;
    u32 *cmd;
    u8 *data;

    cmd = BattleCmd_AllocSlot();
    cmd[0] = 2;

    LOOKUP_SLOT_DATA(g_AyaEquippedWeaponSlot, data);
    cmd[1] = (u32)data;

    if (index >= 0 && index < g_InvActiveListCount) {
        selected = g_InvActiveListItems[index];
    } else {
        selected = 0;
    }

    if (index >= 0 && index < g_InvActiveListCount) {
        g_AyaEquippedWeaponSlot = g_InvActiveListItems[index];
    } else {
        g_AyaEquippedWeaponSlot = 0;
    }

    Inv_SetActiveList(2, &selected);
}
