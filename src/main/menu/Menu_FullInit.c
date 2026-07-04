/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned int g_SavedMenuMode;
extern unsigned int g_MemCardDialogState;
extern unsigned int g_MenuErrorSoundPending;
/* g_GameState lives at 0x800B0CD8, outside the gp window; the incomplete
 * array type keeps -G8 from treating this 4-byte extern as small data and
 * matches retail's materialized-pointer read-modify-write. */
extern unsigned int g_GameState[];

void Menu_SetBattleEquipMode(int);
void Save_ResetMetadataWindow(int);
void Menu_ResetStateFlags(void);
void Inv_SetDefaultItemId(void);
void Draw_InitBuffers(void);
void MenuWidget_InitPoolUnk(void);
void Sys_InitStateBuffer(void);
void Queue_Init(void);
void MemCard_ClearDelayedCallback(void);
void BattleCmd_InitTableCursor(void);
void Inv_BuildItemGridFromCategory(void);
void Save_SelectMetadataWindow(int);
void Save_InitMetadataState(void);
void BattleCmd_LoadWeaponModifiers(void);
void Menu_SaveBgInitFade(void);
void Menu_SetEquipSlotIndex(int);

void Menu_FullInit(void) {
    Menu_SetBattleEquipMode(0);
    Save_ResetMetadataWindow(0);
    g_SavedMenuMode = 0;
    g_MemCardDialogState = 0;
    g_MenuErrorSoundPending = 0;
    Menu_ResetStateFlags();
    Inv_SetDefaultItemId();
    Draw_InitBuffers();
    MenuWidget_InitPoolUnk();
    Sys_InitStateBuffer();
    Queue_Init();
    MemCard_ClearDelayedCallback();
    BattleCmd_InitTableCursor();
    Inv_BuildItemGridFromCategory();
    Save_SelectMetadataWindow(0);
    Save_InitMetadataState();
    BattleCmd_LoadWeaponModifiers();
    Menu_SaveBgInitFade();
    Save_ResetMetadataWindow(1);
    g_GameState[0] |= 0x40000000;
    Menu_SetEquipSlotIndex(1);
}
