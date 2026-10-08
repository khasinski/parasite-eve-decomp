/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory_slots.h"

extern unsigned int g_MenuBattleEquipMode;
extern unsigned int g_SavedMenuMode;

unsigned int Menu_GetActiveMode(void);
void Window_SetBoundsByMode(int mode);
void BattleCmd_SyncActiveAmmo(void);
void Menu_CreateInventoryTabView(void);
void Menu_PlayConfirmSound(void);

void Inventory_OpenAyaItemList(unsigned int arg) {
    g_MenuBattleEquipMode = arg;
    g_SavedMenuMode = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
    BattleCmd_SyncActiveAmmo();
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
    Menu_CreateInventoryTabView();
    Menu_PlayConfirmSound();
}
