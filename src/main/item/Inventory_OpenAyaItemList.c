/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned int g_MenuBattleEquipMode;
extern unsigned int g_SavedMenuMode;
extern unsigned int g_InvItemPtr;
extern unsigned int g_InvSlotLimit;
extern unsigned int g_InvSelectionBits;
extern unsigned int g_InvSelectionBitWords;
extern unsigned short g_AyaInventoryItems[];
extern unsigned int g_AyaItemSelectionBits[];

unsigned int Menu_GetActiveMode(void);
void Window_SetBoundsByMode(int mode);
void BattleCmd_SyncActiveAmmo(void);
int Inv_GetAyaSlotLimit(void);
void Menu_CreateInventoryTabView(void);
void Menu_PlayConfirmSound(void);

void Inventory_OpenAyaItemList(unsigned int arg) {
    g_MenuBattleEquipMode = arg;
    g_SavedMenuMode = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
    BattleCmd_SyncActiveAmmo();
    g_InvItemPtr = (unsigned int)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = (unsigned int)g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
    Menu_CreateInventoryTabView();
    Menu_PlayConfirmSound();
}
