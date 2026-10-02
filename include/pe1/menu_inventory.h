#ifndef PE1_MENU_INVENTORY_H
#define PE1_MENU_INVENTORY_H

#include "pe1/menu_widget.h"

void BoundsCheck_AssertStub(int code);
void MenuWidget_InitPool(void);
void Inventory_OpenAyaItemList(unsigned int mode);

MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode,
                                                     int selectedBase);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void MenuWidget_RestoreSavedCurrentNode(void);
void MenuWidget_DestroyNode(MenuWidgetNode *node);
void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node);
void func_80064A54(MenuWidgetNode *node);
void MenuWidget_NavScrollTo(int selectedBase);
int Menu_InventoryNavigate(MenuWidgetNode *current, MenuWidgetNode *node,
                           u32 flags);
void Menu_CreateBonusPointAllocationView(void);
void Menu_PlayCancelSound(void);
void Menu_SetSwapReturnFlag(void);
void Menu_PlayMoveSound(void);

extern int D_8009CF18, D_8009CFB0;
int MenuWidget_GridCellIndex(MenuWidgetNode *node);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);
void Menu_OpenItemUsePanelAtIndex(int index);
void Menu_OpenSkillSelectionView(void);
void Menu_StepInventoryRoot(int mode, int index, int arg);
void Menu_CreateNotificationDialog(int message, int arg);
void MenuInput_SetPollingPaused(int paused);
void Menu_OnEquipConfirm(MenuWidgetNode *unused, int confirmed);

int Menu_ClampRange(int value);
void Menu_SaveBgInitFade(void);

/* Retail script command numbers. Command 1120 falls through and returns 0. */
enum InvCommand {
    INV_CMD_COUNT_OCCUPIED = 1100,
    INV_CMD_GET_CAPACITY = 1101,
    INV_CMD_COUNT_ITEM = 1102,
    INV_CMD_SET_REBUILD_FILTER = 1103,
    INV_CMD_REBUILD_SLOTS = 1104,
    INV_CMD_SET_CAPACITY = 1105,
    INV_CMD_GET_CURRENT_HP = 1106,
    INV_CMD_GET_MAX_HP = 1107,
    INV_CMD_SET_CURRENT_HP = 1108,
    INV_CMD_GET_REMAINING_AMMO = 1109,
    INV_CMD_GET_AMMO_VALUE = 1110,
    INV_CMD_SET_CURRENT_MP = 1111,
    INV_CMD_REMOVE_ITEM = 1112,
    INV_CMD_OPEN_STORAGE = 1113,
    INV_CMD_COMPUTE_GAMMA = 1114,
    INV_CMD_INIT_MEMCARD = 1115,
    INV_CMD_START_SAVE_FADE = 1116,
    INV_CMD_START_NEW_GAME = 1117,
    INV_CMD_INIT_BONUS_POINTS = 1118,
    INV_CMD_CLEAR_STORAGE_ITEM = 1119,
    INV_CMD_MERGE_STORAGE = 1120
};

int Inv_DispatchCommand(int command, int value, int other, int *unused);
void Menu_CreateContextHelpPanel(void);
void Menu_ComputeGammaLut(int initial, int threshold);
void Menu_SaveBgStartFadeOut(void);
void Menu_SetMemCardConfirmPending(void);
void Menu_InitBonusPointAllocState(int gainedPoints);

void Menu_CopyPromptCodes(u8 *source);
void Menu_SetActionSubmenuSelection(int value);
void MenuWidget_SetColumnLayoutMode(int mode);

extern int g_MenuItemUseMode;
void MenuWidget_ClampScroll(MenuWidgetNode *node);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void MenuWidget_ClearCursorY(MenuWidgetNode *node);
void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *node, int stat);
void Menu_PlayConfirmSound(void);
void func_800490B0(void);
int Menu_StatSlotInputHandler(MenuWidgetNode *node, unsigned int flags);
int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags);

int Menu_CheckItemAffordable(int actionId);
int Menu_GetBattleEquipMode(void);
extern int D_8009CF3C; /* Restricts selected parasite actions when nonzero. */
/* Incomplete arrays retain the retail full-address HP accesses with -G8. */
extern u16 D_800C0E08[]; /* Current HP. */
extern u16 D_800C0E06[]; /* Maximum HP. */

int Menu_StepNameEntry(MenuWidgetNode *parent, unsigned int flags);
void Menu_PlayErrorSound(void);
extern u8 *D_8009CF54;
extern int D_8009D004;

#endif
